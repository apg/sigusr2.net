#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define MAX_PAGES 1024
#define MAX_NAME_LEN 256
#define MAX_VALUE_LEN 512
#define MAX_CONTENT_LEN 32768
#define MAX_METADATA_LEN 4096
#define MAX_SIBLINGS 64
#define MAX_BACKLINKS 64
#define MAX_METADATA_PAIRS 16
#define MAX_TEMPLATE_LEN 65536
#define MAX_OUTPUT_LEN 131072
#define MAX_MARKUP_OUTPUT 131072
#define MAX_PATH_LEN 4096
#define MAX_FILES 2048
#define MAX_FILTERS 16
#define MAX_SAFE_URL_LEN 2048
#define MAX_SAFE_MARKUP_LEN 8192

#ifndef FOLDR_DATA_EXT
#define FOLDR_DATA_EXT ".fmd"
#endif

struct filter {
  char key[MAX_NAME_LEN];
  char value[MAX_VALUE_LEN];
};

struct page {
  char name[MAX_NAME_LEN];
  char source_path[MAX_PATH_LEN];
  char metadata_keys[MAX_METADATA_PAIRS][MAX_NAME_LEN];
  char metadata_values[MAX_METADATA_PAIRS][MAX_VALUE_LEN];
  int metadata_count;
  char content[MAX_CONTENT_LEN];
  int parent;
  int siblings[MAX_SIBLINGS];
  int sibling_count;
  int backlinks[MAX_BACKLINKS];
  int backlink_count;
};

struct file_info {
  char path[MAX_PATH_LEN];
  char relative_path[MAX_PATH_LEN];
};

static struct page pages[MAX_PAGES];
static int page_count = 0;
static struct page *current_pages = pages;
static int current_page_count = 0;
static char template_content[MAX_TEMPLATE_LEN];

struct iteration_context {
  struct page *current_page;
  struct page *root_page;
};

static struct iteration_context iter_context = {NULL, NULL};

static void
panic(const char *msg)
{
  fprintf(stderr, "PANIC: %s\n", msg);
  exit(1);
}

static void
panic_with_path(const char *msg, const char *path)
{
  fprintf(stderr, "PANIC: %s %s\n", msg, path);
  exit(1);
}

static void
panic_with_line(const char *msg, int line_num)
{
  fprintf(stderr, "PANIC: %s at line %d\n", msg, line_num);
  exit(1);
}

static void
template_error(int line_num, const char *msg)
{
  fprintf(stderr, "PANIC: Template syntax error at line %d: %s\n", line_num, msg);
  exit(1);
}

static size_t
safe_strcpy(char *dst, const char *src, size_t dstsize)
{
  if (dstsize == 0) { return strlen(src); }
  return snprintf(dst, dstsize, "%s", src);
}

static size_t
safe_strcat(char *dst, const char *src, size_t dstsize)
{
  size_t dstlen = strlen(dst);
  if (dstlen >= dstsize) { return dstlen + strlen(src); }
  return dstlen + snprintf(dst + dstlen, dstsize - dstlen, "%s", src);
}

static char *
trim_whitespace(char *str)
{
  char *end;
  while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;
  if (*str == 0) { return str; }
  end = str + strlen(str) - 1;
  while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
  end[1] = '\0';
  return str;
}

static const char *
get_metadata(const struct page *page, const char *key)
{
  if (page == NULL) {
    panic("NULL page pointer in get_metadata");
  }
  if (key == NULL) {
    panic("NULL key pointer in get_metadata");
  }

  for (int i = 0; i < page->metadata_count; i++) {
    if (strcmp(page->metadata_keys[i], key) == 0) {
      return page->metadata_values[i];
    }
  }
  return NULL;
}

static void
add_metadata(struct page *page, const char *key, const char *value)
{
  if (page == NULL) {
    panic("NULL page pointer in add_metadata");
  }
  if (key == NULL) {
    panic("NULL key pointer in add_metadata");
  }
  if (value == NULL) {
    panic("NULL value pointer in add_metadata");
  }

  if (page->metadata_count >= MAX_METADATA_PAIRS) {
    panic("Too many metadata pairs");
  }
  safe_strcpy(page->metadata_keys[page->metadata_count], key, MAX_NAME_LEN);
  safe_strcpy(page->metadata_values[page->metadata_count], value, MAX_VALUE_LEN);
  page->metadata_count++;
}

static int
parse_filter(const char *filter_str, struct filter *filter)
{
  // Use front matter parsing logic for a single line
  char *colon_in_orig = strchr(filter_str, ':');
  if (colon_in_orig == NULL) {
    return 0;
  }

  char line[512];
  safe_strcpy(line, filter_str, sizeof(line));

  // Find colon position in the copied string
  char *colon = strchr(line, ':');
  if (colon == NULL) {
    return 0;  // This shouldn't happen
  }

  *colon = '\0';
  char *key = trim_whitespace(line);
  char *value = trim_whitespace(colon + 1);

  safe_strcpy(filter->key, key, MAX_NAME_LEN);
  safe_strcpy(filter->value, value, MAX_VALUE_LEN);

  return 1;
}

static int
page_matches_filters(struct page *page, struct filter *filters, int filter_count)
{
  for (int i = 0; i < filter_count; i++) {
    const char *page_value = get_metadata(page, filters[i].key);
    if (page_value == NULL) {
      return 0;  // Page doesn't have this key
    }

    if (strcmp(page_value, filters[i].value) != 0) {
      return 0;  // Value doesn't match (already trimmed by front matter parser)
    }
  }
  return 1;  // All filters match
}

static void
parse_front_matter(struct page *page, const char *text)
{
  char line[1024];
  const char *ptr = text;
  const char *line_end;

  if (strncmp(ptr, "---\n", 4) != 0) {
    panic("Front matter must start with ---");
  }
  ptr += 4;

  while ((line_end = strchr(ptr, '\n')) != NULL) {
    size_t line_len = line_end - ptr;
    if (line_len >= sizeof(line)) {
      panic("Front matter line too long");
    }

    memcpy(line, ptr, line_len);
    line[line_len] = '\0';

    if (strcmp(line, "---") == 0) {
      return;
    }

    char *colon = strchr(line, ':');
    if (colon != NULL) {
      *colon = '\0';
      char *key = trim_whitespace(line);
      char *value = trim_whitespace(colon + 1);
      add_metadata(page, key, value);
    }
    ptr = line_end + 1;
  }

  panic("Front matter not properly terminated with ---");
}

static int
extract_links(const char *content, char links[][MAX_NAME_LEN])
{
  const char *ptr = content;
  const char *start, *end;
  int link_count = 0;

  while ((start = strstr(ptr, "[[")) != NULL && link_count < MAX_BACKLINKS) {
    start += 2;
    end = strstr(start, "]]");
    if (end == NULL) break;

    size_t len = end - start;
    if (len >= MAX_NAME_LEN) {
      panic("Link name too long");
    }

    memcpy(links[link_count], start, len);
    links[link_count][len] = '\0';
    link_count++;

    ptr = end + 2;
  }

  return link_count;
}

static int
compare_pages_by_name(const void *a, const void *b)
{
  const struct page *page_a = (const struct page *)a;
  const struct page *page_b = (const struct page *)b;
  return strcmp(page_a->name, page_b->name);
}

// Global variables for custom sorting
static char sort_key[MAX_NAME_LEN];
static int sort_ascending;

static int
compare_pages_by_metadata(const void *a, const void *b)
{
  const struct page *page_a = (const struct page *)a;
  const struct page *page_b = (const struct page *)b;
  
  const char *value_a = get_metadata(page_a, sort_key);
  const char *value_b = get_metadata(page_b, sort_key);
  
  // Handle missing metadata (treat as empty string)
  if (!value_a) value_a = "";
  if (!value_b) value_b = "";
  
  int result = strcmp(value_a, value_b);
  
  // Reverse if descending order
  if (!sort_ascending) {
    result = -result;
  }
  
  return result;
}

static int
compare_ints(const void *a, const void *b)
{
  int int_a = *(const int *)a;
  int int_b = *(const int *)b;
  return int_a - int_b;
}

static int
find_page_by_name(const char *name)
{
  // Linear search since pages might be sorted by metadata, not by name
  for (int i = 0; i < page_count; i++) {
    if (strcmp(pages[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

static void
build_category_tree(void)
{
  for (int i = 0; i < page_count; i++) {
    struct page *page = &pages[i];
    const char *category = get_metadata(page, "category");

    page->parent = -1;
    page->sibling_count = 0;

    if (category == NULL) {
      continue;
    }

    char parent_name[MAX_NAME_LEN];
    const char *last_slash = strrchr(category, '/');
    if (last_slash != NULL) {
      size_t parent_len = last_slash - category;
      if (parent_len >= MAX_NAME_LEN) {
        panic("Parent category name too long");
      }
      memcpy(parent_name, category, parent_len);
      parent_name[parent_len] = '\0';

      int parent_idx = find_page_by_name(parent_name);
      if (parent_idx != -1) {
        page->parent = parent_idx;
      }
    }

    for (int j = 0; j < page_count; j++) {
      if (i == j) continue;

      const char *other_category = get_metadata(&pages[j], "category");
      if (other_category != NULL && strcmp(category, other_category) == 0) {
        if (page->sibling_count >= MAX_SIBLINGS) {
          panic("Too many siblings");
        }
        page->siblings[page->sibling_count++] = j;
      }
    }
  }

  for (int i = 0; i < page_count; i++) {
    if (pages[i].sibling_count > 1) {
      qsort(pages[i].siblings, pages[i].sibling_count, sizeof(int), compare_ints);
    }
  }
}

static void
build_backlinks(void)
{
  for (int i = 0; i < page_count; i++) {
    pages[i].backlink_count = 0;
  }

  for (int i = 0; i < page_count; i++) {
    char links[MAX_BACKLINKS][MAX_NAME_LEN];
    int link_count = extract_links(pages[i].content, links);

    for (int j = 0; j < link_count; j++) {
      int target_idx = find_page_by_name(links[j]);
      if (target_idx != -1 && target_idx != i) {
        struct page *target = &pages[target_idx];
        if (target->backlink_count >= MAX_BACKLINKS) {
          panic("Too many backlinks");
        }
        target->backlinks[target->backlink_count++] = i;
      }
    }
  }

  for (int i = 0; i < page_count; i++) {
    if (pages[i].backlink_count > 1) {
      qsort(pages[i].backlinks, pages[i].backlink_count, sizeof(int), compare_ints);
    }
  }
}

static void
print_page_structure(void)
{
  printf("Pages (%d total):\n", page_count);
  printf("================\n\n");

  for (int i = 0; i < page_count; i++) {
    struct page *page = &pages[i];

    printf("Page %d: %s\n", i, page->name);
    printf("  Metadata:\n");
    for (int j = 0; j < page->metadata_count; j++) {
      printf("    %s: %s\n", page->metadata_keys[j], page->metadata_values[j]);
    }

    printf("  Parent: ");
    if (page->parent == -1) {
      printf("root\n");
    }
    else if (page->parent >= 0 && page->parent < page_count) {
      printf("%s (index %d)\n", pages[page->parent].name, page->parent);
    }
    else {
      printf("INVALID PARENT INDEX %d\n", page->parent);
    }

    printf("  Siblings (%d): ", page->sibling_count);
    for (int j = 0; j < page->sibling_count; j++) {
      int sibling_idx = page->siblings[j];
      if (sibling_idx >= 0 && sibling_idx < page_count) {
        printf("%s", pages[sibling_idx].name);
      } else {
        printf("INVALID_SIBLING[%d]", sibling_idx);
      }
      if (j < page->sibling_count - 1) printf(", ");
    }
    printf("\n");

    printf("  Backlinks (%d): ", page->backlink_count);
    for (int j = 0; j < page->backlink_count; j++) {
      int backlink_idx = page->backlinks[j];
      if (backlink_idx >= 0 && backlink_idx < page_count) {
        printf("%s", pages[backlink_idx].name);
      } else {
        printf("INVALID_BACKLINK[%d]", backlink_idx);
      }
      if (j < page->backlink_count - 1) printf(", ");
    }
    printf("\n\n");
  }
}

static void
load_template(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    char msg[512];
    snprintf(msg, sizeof(msg), "Cannot open template file: %s", filename);
    panic(msg);
  }

  size_t bytes_read = fread(template_content, 1, MAX_TEMPLATE_LEN - 1, file);
  if (ferror(file)) {
    fclose(file);
    panic_with_path("Error reading template file:", filename);
  }
  else {
    template_content[bytes_read] = '\0';
    fclose(file);
  }
}

static void
html_escape(const char *input, char *output, size_t output_size)
{
  const char *src = input;
  char *dst = output;
  size_t remaining = output_size - 1;

  while (*src && remaining > 4) {
    switch (*src) {
    case '<':
      if (remaining >= 4) {
        memcpy(dst, "&lt;", 4);
        dst += 4;
        remaining -= 4;
      }
      break;
    case '>':
      if (remaining >= 4) {
        memcpy(dst, "&gt;", 4);
        dst += 4;
        remaining -= 4;
      }
      break;
    case '&':
      if (remaining >= 5) {
        memcpy(dst, "&amp;", 5);
        dst += 5;
        remaining -= 5;
      }
      break;
    case '"':
      if (remaining >= 6) {
        memcpy(dst, "&quot;", 6);
        dst += 6;
        remaining -= 6;
      }
      break;
    case '\'':
      if (remaining >= 6) {
        memcpy(dst, "&#x27;", 6);
        dst += 6;
        remaining -= 6;
      }
      break;
    default:
      *dst++ = *src;
      remaining--;
      break;
    }
    src++;
  }

  *dst = '\0';
}


typedef int (*template_handler_func)(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);

const char *get_variable_value(struct page *page, const char *var_name);
char *render_template_block(struct page *page, const char *template_text);

int handle_literal(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_variable(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_raw_variable(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_markup_variable(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_iteration_block(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_conditional_block(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_file_include(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);
int handle_file_include_escaped(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num);

struct template_prefix {
  const char *prefix;
  const char *suffix;
  int prefix_len;
  int suffix_len;
  int escape_html;
  int is_literal;
  const char *literal_output;
  template_handler_func handler;
};

static struct template_prefix prefixes[] = {
  {"\\{{", "", 3, 0, 0, 1, "{{", handle_literal},
  {"\\}}", "", 3, 0, 0, 1, "}}", handle_literal},
  {"{{^", "}}", 3, 2, 0, 0, NULL, handle_file_include_escaped},
  {"{{<", "}}", 3, 2, 0, 0, NULL, handle_file_include},
  {"{{#", "", 3, 0, 0, 0, NULL, handle_iteration_block},
  {"{{?", "", 3, 0, 0, 0, NULL, handle_conditional_block},
  {"{{!", "}}", 3, 2, 0, 0, NULL, handle_markup_variable},
  {"{{&", "}}", 3, 2, 0, 0, NULL, handle_raw_variable},
  {"{{", "}}", 2, 2, 1, 0, NULL, handle_variable},
  {NULL, NULL, 0, 0, 0, 0, NULL, NULL}
};

typedef int (*markup_handler_func)(const char *src, char *dst, size_t remaining, const char **next_src);

char *process_inline_markup(const char *text);
static int process_inline_markup_to_buffer_depth(const char *text, char *output, size_t output_size, int depth);
static int process_inline_markup_to_buffer(const char *text, char *output, size_t output_size);
int handle_simple_markup(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_wiki_link(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_link(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_image(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_anchor(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_ellipses(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_em_dash(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_en_dash(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_curly_quotes(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_line_break(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_footnote_ref(const char *src, char *dst, size_t remaining, const char **next_src);
int handle_new_anchor(const char *src, char *dst, size_t remaining, const char **next_src);

struct inline_markup_rule {
  const char *prefix;
  const char *suffix;
  int prefix_len;
  int suffix_len;
  const char *html_open;
  const char *html_close;
  int process_content;
  markup_handler_func handler;
};

static struct inline_markup_rule inline_rules[] = {
  {"```", "```", 3, 3, "<code>", "</code>", 0, handle_simple_markup},
  {"``", "``", 2, 2, "<code>", "</code>", 0, handle_simple_markup},
  {"`", "`", 1, 1, "<code>", "</code>", 0, handle_simple_markup},
  {"**", "**", 2, 2, "<strong>", "</strong>", 1, handle_simple_markup},
  {"__", "__", 2, 2, "<strong>", "</strong>", 1, handle_simple_markup},
  {"*", "*", 1, 1, "<em>", "</em>", 1, handle_simple_markup},
  {"_", "_", 1, 1, "<em>", "</em>", 1, handle_simple_markup},
  {"...", "", 3, 0, NULL, NULL, 0, handle_ellipses},
  {"---", "", 3, 0, NULL, NULL, 0, handle_em_dash},
  {"--", "", 2, 0, NULL, NULL, 0, handle_en_dash},
  {"\\", "", 1, 0, NULL, NULL, 0, handle_line_break},
  {"[[", "]]", 2, 2, NULL, NULL, 0, handle_wiki_link},
  {"[^", "]", 2, 1, NULL, NULL, 0, handle_footnote_ref},
  {"[#", "]", 2, 1, NULL, NULL, 0, handle_new_anchor},
  {"[", ")", 1, 1, NULL, NULL, 0, handle_link},
  {"![", ")", 2, 1, NULL, NULL, 0, handle_image},
  {"\"", "", 1, 0, NULL, NULL, 0, handle_curly_quotes},
  {"'", "", 1, 0, NULL, NULL, 0, handle_curly_quotes},
  {NULL, NULL, 0, 0, NULL, NULL, 0, NULL}
};


typedef int (*block_handler_func)(const char **lines, int line_count, int *current_line, char *output, size_t output_size);

int handle_header(const char **lines, int line_count, int *current_line, char *output, size_t output_size);
int handle_list(const char **lines, int line_count, int *current_line, char *output, size_t output_size);
int handle_code_block(const char **lines, int line_count, int *current_line, char *output, size_t output_size);
int handle_blockquote(const char **lines, int line_count, int *current_line, char *output, size_t output_size);
int handle_indented_code(const char **lines, int line_count, int *current_line, char *output, size_t output_size);
int handle_paragraph(const char **lines, int line_count, int *current_line, char *output, size_t output_size);
int handle_footnote_def(const char **lines, int line_count, int *current_line, char *output, size_t output_size);

struct block_markup_rule {
  const char *prefix;
  int min_prefix_len;
  const char *html_open;
  const char *html_close;
  int process_content;
  block_handler_func handler;
};

static struct block_markup_rule block_rules[] = {
  {"######", 6, "<h6>", "</h6>", 1, handle_header},
  {"#####", 5, "<h5>", "</h5>", 1, handle_header},
  {"####", 4, "<h4>", "</h4>", 1, handle_header},
  {"###", 3, "<h3>", "</h3>", 1, handle_header},
  {"##", 2, "<h2>", "</h2>", 1, handle_header},
  {"#", 1, "<h1>", "</h1>", 1, handle_header},
  {"> ", 2, "<blockquote><p>", "</p></blockquote>", 1, handle_blockquote},
  {"```", 3, "<pre><code", "</code></pre>", 0, handle_code_block},
  {"    ", 4, "<pre><code>", "</code></pre>", 0, handle_indented_code},
  {"* ", 2, "<ul><li>", "</li></ul>", 1, handle_list},
  {"- ", 2, "<ul><li>", "</li></ul>", 1, handle_list},
  {NULL, 0, NULL, NULL, 0, NULL}
};

int
handle_simple_markup(const char *src, char *dst, size_t remaining, const char **next_src)
{
  for (int i = 0; inline_rules[i].prefix != NULL; i++) {
    if (strncmp(src, inline_rules[i].prefix, inline_rules[i].prefix_len) == 0 &&
        inline_rules[i].handler == handle_simple_markup) {

      const char *content_start = src + inline_rules[i].prefix_len;
      const char *content_end = strstr(content_start, inline_rules[i].suffix);

      if (content_end != NULL) {
        size_t written = 0;

        if (inline_rules[i].html_open) {
          size_t open_len = strlen(inline_rules[i].html_open);
          if (open_len < remaining) {
            memcpy(dst, inline_rules[i].html_open, open_len);
            dst += open_len;
            written += open_len;
            remaining -= open_len;
          }
        }

        size_t content_len = content_end - content_start;
        if (inline_rules[i].process_content) {
          char content_buf[1024];
          snprintf(content_buf, sizeof(content_buf), "%.*s", (int)content_len, content_start);
          char processed_buf[2048];

          // Process escapes and simple links within emphasis content
          const char *inner_src = content_buf;
          char *inner_dst = processed_buf;
          size_t inner_remaining = sizeof(processed_buf) - 1;

          while (*inner_src && inner_remaining > 1) {
            if (*inner_src == '\\' && inner_src[1] != '\0' && ispunct(inner_src[1])) {
              *inner_dst++ = inner_src[1];
              inner_src += 2;
              inner_remaining--;
              continue;
            }
            
            // Handle basic links [text](url) without recursion
            if (*inner_src == '[') {
              const char *link_end = strchr(inner_src + 1, ']');
              if (link_end && link_end[1] == '(') {
                // Find matching closing parenthesis, handling nested parentheses
                const char *url_start = link_end + 2;
                const char *ptr = url_start;
                int paren_count = 1;  // Start with 1 for the opening paren
                const char *url_end = NULL;
                int max_chars = 1000;  // Prevent infinite loops
                
                while (*ptr && paren_count > 0 && max_chars-- > 0) {
                  if (*ptr == '\\' && ptr[1] != '\0') {
                    ptr += 2;  // Skip escaped character
                    continue;
                  }
                  if (*ptr == '(') {
                    paren_count++;
                  } else if (*ptr == ')') {
                    paren_count--;
                    if (paren_count == 0) {
                      url_end = ptr;
                      break;
                    }
                  }
                  ptr++;
                }
                
                if (url_end) {
                  // Found a complete link, process it
                  size_t text_len = link_end - (inner_src + 1);
                  size_t url_len = url_end - url_start;
                  
                  // Simple link HTML generation
                  int link_written = snprintf(inner_dst, inner_remaining, "<a href=\"%.*s\">%.*s</a>",
                                              (int)url_len, url_start, (int)text_len, inner_src + 1);
                  if (link_written > 0 && (size_t)link_written < inner_remaining) {
                    inner_dst += link_written;
                    inner_remaining -= link_written;
                    inner_src = url_end + 1;
                    continue;
                  }
                }
              }
            }
            
            *inner_dst++ = *inner_src++;
            inner_remaining--;
          }
          *inner_dst = '\0';

          size_t processed_len = strlen(processed_buf);
          if (processed_len < remaining) {
            memcpy(dst, processed_buf, processed_len);
            dst += processed_len;
            written += processed_len;
            remaining -= processed_len;
          }
        }
        else {
          char processed_content[1024];
          char content_buf[1024];
          snprintf(content_buf, sizeof(content_buf), "%.*s", (int)content_len, content_start);

          // Process escape sequences manually
          const char *inner_src = content_buf;
          char *inner_dst = processed_content;
          size_t inner_remaining = sizeof(processed_content) - 1;

          while (*inner_src && inner_remaining > 1) {
            if (*inner_src == '\\' && inner_src[1] != '\0' && ispunct(inner_src[1])) {
              *inner_dst++ = inner_src[1];
              inner_src += 2;
              inner_remaining--;
              continue;
            }
            *inner_dst++ = *inner_src++;
            inner_remaining--;
          }
          *inner_dst = '\0';

          char escaped_content[2048];
          html_escape(processed_content, escaped_content, sizeof(escaped_content));
          size_t escaped_len = strlen(escaped_content);
          if (escaped_len < remaining) {
            memcpy(dst, escaped_content, escaped_len);
            dst += escaped_len;
            written += escaped_len;
            remaining -= escaped_len;
          }
        }

        if (inline_rules[i].html_close) {
          size_t close_len = strlen(inline_rules[i].html_close);
          if (close_len < remaining) {
            memcpy(dst, inline_rules[i].html_close, close_len);
            written += close_len;
          }
        }

        *next_src = content_end + inline_rules[i].suffix_len;
        return written;
      }
    }
  }
  return 0;
}

int
handle_wiki_link(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *content_start = src + 2;
  const char *content_end = strstr(content_start, "]]");

  if (content_end != NULL) {
    size_t content_len = content_end - content_start;

    if (content_len > MAX_SAFE_MARKUP_LEN) {
      panic("Wiki link content too long for safe rendering");
    }

    // Extract page name from wiki link content
    char page_name[MAX_NAME_LEN];
    if (content_len >= MAX_NAME_LEN) {
      panic("Wiki link page name too long");
    }
    memcpy(page_name, content_start, content_len);
    page_name[content_len] = '\0';

    // Look up the page to get its title
    const char *link_text = page_name; // fallback to page name
    int page_idx = find_page_by_name(page_name);
    if (page_idx >= 0) {
      const char *title = get_metadata(&pages[page_idx], "title");
      if (title && strlen(title) > 0) {
        link_text = title;
      }
    }

    char link_html[512];
    snprintf(link_html, sizeof(link_html), "<a href=\"/%s.html\">%s</a>",
             page_name, link_text);

    size_t html_len = strlen(link_html);
    if (html_len < remaining) {
      memcpy(dst, link_html, html_len);
      *next_src = content_end + 2;
      return html_len;
    }
  }
  return 0;
}

int
handle_link(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *content_start = src + 1;  // Skip '['
  const char *bracket_end = strchr(content_start, ']');

  if (bracket_end != NULL && bracket_end[1] == '(') {
    const char *paren_start = bracket_end + 2;  // Skip "]("
    const char *paren_end = NULL;
    
    // Find matching closing parenthesis, handling nested parentheses
    const char *ptr = paren_start;
    int paren_count = 1;  // Start with 1 for the opening paren we're inside
    int max_chars = 1000;  // Prevent infinite loops
    
    while (*ptr && paren_count > 0 && max_chars-- > 0) {
      if (*ptr == '\\' && ptr[1] != '\0') {
        ptr += 2;  // Skip escaped character
        continue;
      }
      if (*ptr == '(') {
        paren_count++;
      } else if (*ptr == ')') {
        paren_count--;
        if (paren_count == 0) {
          paren_end = ptr;
          break;
        }
      }
      ptr++;
    }

    if (paren_end != NULL) {
      size_t text_len = bracket_end - content_start;
      size_t url_len = paren_end - paren_start;

      if (text_len > MAX_SAFE_MARKUP_LEN) {
        panic("Link text too long for safe rendering");
      }
      if (url_len > MAX_SAFE_URL_LEN) {
        panic("Link URL too long for safe rendering");
      }

      // Process escapes in link text
      char processed_text[256];
      char text_buf[256];
      snprintf(text_buf, sizeof(text_buf), "%.*s", (int)text_len, content_start);

      const char *text_src = text_buf;
      char *text_dst = processed_text;
      while (*text_src) {
        if (*text_src == '\\' && text_src[1] != '\0' && ispunct(text_src[1])) {
          *text_dst++ = text_src[1];
          text_src += 2;
        } else {
          *text_dst++ = *text_src++;
        }
      }
      *text_dst = '\0';

      // Process escapes in URL
      char processed_url[256];
      char url_buf[256];
      snprintf(url_buf, sizeof(url_buf), "%.*s", (int)url_len, paren_start);

      const char *url_src = url_buf;
      char *url_dst = processed_url;
      while (*url_src) {
        if (*url_src == '\\' && url_src[1] != '\0' && ispunct(url_src[1])) {
          *url_dst++ = url_src[1];
          url_src += 2;
        } else {
          *url_dst++ = *url_src++;
        }
      }
      *url_dst = '\0';

      char link_html[512];
      snprintf(link_html, sizeof(link_html), "<a href=\"%s\">%s</a>",
               processed_url, processed_text);

      size_t html_len = strlen(link_html);
      if (html_len < remaining) {
        memcpy(dst, link_html, html_len);
        *next_src = paren_end + 1;
        return html_len;
      }
    }
  }
  return 0;
}

int
handle_image(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *alt_start = src + 2;  // Skip "!["
  const char *bracket_end = strchr(alt_start, ']');

  if (bracket_end != NULL && bracket_end[1] == '(') {
    const char *url_start = bracket_end + 2;  // Skip "]("
    const char *paren_end = strchr(url_start, ')');

    if (paren_end != NULL) {
      size_t alt_len = bracket_end - alt_start;
      size_t url_len = paren_end - url_start;

      if (url_len > MAX_SAFE_URL_LEN) {
        panic("Image URL too long for safe rendering");
      }
      if (alt_len > MAX_SAFE_MARKUP_LEN) {
        panic("Image alt text too long for safe rendering");
      }

      char img_html[512];
      snprintf(img_html, sizeof(img_html), "<img src=\"%.*s\" alt=\"%.*s\">",
               (int)url_len, url_start, (int)alt_len, alt_start);

      size_t html_len = strlen(img_html);
      if (html_len < remaining) {
        memcpy(dst, img_html, html_len);
        *next_src = paren_end + 1;
        return html_len;
      }
    }
  }
  return 0;
}

int
handle_anchor(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *anchor_start = src + 2;
  const char *anchor_end = anchor_start;
  while (*anchor_end && *anchor_end != ' ' && *anchor_end != '\n' && *anchor_end != '\t') {
    anchor_end++;
  }

  size_t anchor_len = anchor_end - anchor_start;
  char anchor_html[256];
  snprintf(anchor_html, sizeof(anchor_html), "<a id=\"%.*s\"></a>", (int)anchor_len, anchor_start);

  size_t html_len = strlen(anchor_html);
  if (html_len < remaining) {
    memcpy(dst, anchor_html, html_len);
    *next_src = anchor_end;
    return html_len;
  }
  return 0;
}

int
handle_ellipses(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *ellipses = "&hellip;";
  size_t len = strlen(ellipses);
  if (len < remaining) {
    memcpy(dst, ellipses, len);
    *next_src = src + 3;  // Skip "..."
    return len;
  }
  return 0;
}

int
handle_em_dash(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *em_dash = "&mdash;";
  size_t len = strlen(em_dash);
  if (len < remaining) {
    memcpy(dst, em_dash, len);
    *next_src = src + 3;  // Skip "---"
    return len;
  }
  return 0;
}

int
handle_en_dash(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *en_dash = "&ndash;";
  size_t len = strlen(en_dash);
  if (len < remaining) {
    memcpy(dst, en_dash, len);
    *next_src = src + 2;  // Skip "--"
    return len;
  }
  return 0;
}

static int
is_inside_html_tag(const char *start, const char *current)
{
  // Look backwards for < and > to see if we're inside an HTML tag
  const char *ptr = current - 1;
  int found_open_bracket = 0;
  
  while (ptr >= start) {
    if (*ptr == '>') {
      // Found closing bracket before opening, we're not in a tag
      return 0;
    }
    if (*ptr == '<') {
      found_open_bracket = 1;
      break;
    }
    ptr--;
  }
  
  return found_open_bracket;
}

int
handle_curly_quotes(const char *src, char *dst, size_t remaining, const char **next_src)
{
  // Skip quote conversion if we're inside HTML tags
  // This is a heuristic - we look for the start of the current text block
  // A more robust solution would require passing context from the parser
  const char *line_start = src;
  while (line_start > src - 200 && line_start[-1] != '\n' && line_start[-1] != '\0') {
    line_start--;
  }
  
  if (is_inside_html_tag(line_start, src)) {
    return 0;  // Don't convert quotes inside HTML tags
  }

  static int in_double_quote = 0;
  static int in_single_quote = 0;

  const char *quote_html;

  if (*src == '"') {
    if (in_double_quote) {
      quote_html = "&rdquo;";  // Right double quote
      in_double_quote = 0;
    }
    else {
      quote_html = "&ldquo;";  // Left double quote
      in_double_quote = 1;
    }
  }
  else if (*src == '\'') {
    if (in_single_quote) {
      quote_html = "&rsquo;";  // Right single quote
      in_single_quote = 0;
    }
    else {
      quote_html = "&lsquo;";  // Left single quote
      in_single_quote = 1;
    }
  }
  else {
    return 0;
  }

  size_t len = strlen(quote_html);
  if (len < remaining) {
    memcpy(dst, quote_html, len);
    *next_src = src + 1;
    return len;
  }
  return 0;
}

int
handle_line_break(const char *src, char *dst, size_t remaining, const char **next_src)
{
  // Check if backslash is followed by newline
  if (src[1] == '\n') {
    const char *br = "<br />";
    size_t len = strlen(br);
    if (len < remaining) {
      memcpy(dst, br, len);
      *next_src = src + 2;  // Skip "\\\n"
      return len;
    }
  }
  return 0;
}

int
handle_footnote_ref(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *ref_start = src + 2;  // Skip "[^"
  const char *ref_end = strchr(ref_start, ']');

  if (ref_end != NULL) {
    size_t ref_len = ref_end - ref_start;
    char footnote_html[512];
    snprintf(footnote_html, sizeof(footnote_html),
             "<sup><a id=\"ref-%.*s\" href=\"#fn%.*s\">%.*s</a></sup>",
             (int)ref_len, ref_start, (int)ref_len, ref_start, (int)ref_len, ref_start);

    size_t html_len = strlen(footnote_html);
    if (html_len < remaining) {
      memcpy(dst, footnote_html, html_len);
      *next_src = ref_end + 1;
      return html_len;
    }
  }
  return 0;
}

int
handle_new_anchor(const char *src, char *dst, size_t remaining, const char **next_src)
{
  const char *anchor_start = src + 2;  // Skip "[#"
  const char *anchor_end = strchr(anchor_start, ']');

  if (anchor_end != NULL) {
    size_t anchor_len = anchor_end - anchor_start;
    // Validate that anchor contains only valid characters (alphanumeric and -)
    for (size_t i = 0; i < anchor_len; i++) {
      char c = anchor_start[i];
      if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-')) {
        return 0;  // Invalid character
      }
    }

    char anchor_html[256];
    snprintf(anchor_html, sizeof(anchor_html), "<a id=\"%.*s\"></a>", (int)anchor_len, anchor_start);

    size_t html_len = strlen(anchor_html);
    if (html_len < remaining) {
      memcpy(dst, anchor_html, html_len);
      *next_src = anchor_end + 1;
      return html_len;
    }
  }
  return 0;
}

static int
split_into_lines(const char *text, const char **lines, int max_lines)
{
  if (text == NULL) {
    panic("NULL text pointer in split_into_lines");
  }
  if (lines == NULL) {
    panic("NULL lines pointer in split_into_lines");
  }

  static char line_buffer[MAX_MARKUP_OUTPUT];
  safe_strcpy(line_buffer, text, MAX_MARKUP_OUTPUT);

  int line_count = 0;
  char *current_pos = line_buffer;
  
  while (*current_pos && line_count < max_lines) {
    lines[line_count++] = current_pos;
    
    // Find the next newline or end of string
    char *next_newline = strchr(current_pos, '\n');
    if (next_newline) {
      *next_newline = '\0';  // Null-terminate this line
      current_pos = next_newline + 1;  // Move to start of next line
    } else {
      // This is the last line, no more to process
      break;
    }
  }
  return line_count;
}

int
handle_header(const char **lines, int line_count, int *current_line, char *output, size_t output_size)
{
  if (*current_line >= line_count) { return 0; }

  const char *line = lines[*current_line];
  int hash_count = 0;
  while (line[hash_count] == '#' && hash_count < 6) {
    hash_count++;
  }

  if (hash_count > 0 && line[hash_count] == ' ') {
    const char *content = line + hash_count + 1;

    char header_content[1024];
    safe_strcpy(header_content, content, sizeof(header_content));

    // Check if there's a blank line after the header (next line is empty or whitespace only)
    int has_blank_line = 0;
    if (*current_line + 1 < line_count) {
      const char *next_line = lines[*current_line + 1];
      // A line is considered blank if it's empty or contains only whitespace
      const char *p = next_line;
      while (*p && isspace(*p)) p++;
      has_blank_line = (*p == '\0');
      
      // If there's no blank line, include the next line as part of the header content
      if (!has_blank_line) {
        char extended_content[2048];
        snprintf(extended_content, sizeof(extended_content), "%s %s", header_content, next_line);
        safe_strcpy(header_content, extended_content, sizeof(header_content));
        (*current_line)++; // Skip the next line since we consumed it
      }
    } else {
      // If this is the last line, we consider it acceptable
      has_blank_line = 1;
    }

    // Check for custom anchor [#anchor-id] AFTER determining final content
    char anchor_id[128] = "";
    char *anchor_start = strstr(header_content, " [#");
    if (anchor_start != NULL) {
      char *anchor_end = strchr(anchor_start + 3, ']');
      if (anchor_end != NULL) {
        // Extract anchor ID
        size_t anchor_len = anchor_end - anchor_start - 3;
        snprintf(anchor_id, sizeof(anchor_id), "%.*s", (int)anchor_len, anchor_start + 3);

        // Remove anchor syntax from content
        *anchor_start = '\0';
      }
    }

    char *processed = process_inline_markup(header_content);

    int written;
    if (strlen(anchor_id) > 0) {
      written = snprintf(output, output_size, "<h%d id=\"%s\">%s</h%d>",
                        hash_count, anchor_id, processed, hash_count);
    } else {
      // Auto-generate anchor from header text (convert to lowercase, replace spaces with hyphens)
      char auto_anchor[128];
      const char *src = processed;
      char *dst = auto_anchor;
      int prev_was_space = 0;

      while (*src && dst < auto_anchor + sizeof(auto_anchor) - 1) {
        if (isalnum(*src)) {
          *dst++ = tolower(*src);
          prev_was_space = 0;
        } else if (!prev_was_space && dst > auto_anchor) {
          *dst++ = '-';
          prev_was_space = 1;
        }
        src++;
      }
      if (dst > auto_anchor && dst[-1] == '-') dst--;  // Remove trailing dash
      *dst = '\0';

      written = snprintf(output, output_size, "<h%d id=\"%s\">%s</h%d>",
                        hash_count, auto_anchor, processed, hash_count);
    }

    (*current_line)++;
    return written > 0 ? written : 0;
  }
  return 0;
}

int
handle_list(const char **lines, int line_count, int *current_line, char *output, size_t output_size)
{
  if (*current_line >= line_count) { return 0; }

  char *dst = output;
  size_t remaining = output_size;
  int written = 0;

  const char *first_line = lines[*current_line];
  int is_ordered = (first_line[0] >= '1' && first_line[0] <= '9');

  if (is_ordered) {
    // Parse the full number, not just the first digit
    int start_num = 0;
    const char *ptr = first_line;
    while (*ptr >= '0' && *ptr <= '9') {
      start_num = start_num * 10 + (*ptr - '0');
      ptr++;
    }
    int len = snprintf(dst, remaining, "<ol start=\"%d\">", start_num);
    dst += len;
    written += len;
    remaining -= len;
  }
  else {
    int len = snprintf(dst, remaining, "<ul>");
    dst += len;
    written += len;
    remaining -= len;
  }

  while (*current_line < line_count) {
    const char *line = lines[*current_line];
    const char *content = NULL;

    if ((line[0] == '*' || line[0] == '-') && line[1] == ' ') {
      content = line + 2;
    }
    else if (is_ordered && line[0] >= '1' && line[0] <= '9' &&
               strchr(line, '.') != NULL && strchr(line, '.')[1] == ' ') {
      content = strchr(line, '.') + 2;
    }
    else {
      break;
    }

    char *processed = process_inline_markup(content);
    int len = snprintf(dst, remaining, "<li>%s</li>", processed);
    dst += len;
    written += len;
    remaining -= len;
    (*current_line)++;
  }

  if (is_ordered) {
    int len = snprintf(dst, remaining, "</ol>");
    written += len;
  }
  else {
    int len = snprintf(dst, remaining, "</ul>");
    written += len;
  }

  return written;
}

int
handle_code_block(const char **lines, int line_count, int *current_line, char *output, size_t output_size)
{
  if (*current_line >= line_count) { return 0; }

  const char *first_line = lines[*current_line];
  if (strncmp(first_line, "```", 3) != 0) { return 0; }

  const char *lang = first_line + 3;
  char *dst = output;
  size_t remaining = output_size;
  int written = 0;

  int len;
  if (strlen(lang) > 0) {
    len = snprintf(dst, remaining, "<pre><code class=\"language-%s\">", lang);
  }
  else {
    len = snprintf(dst, remaining, "<pre><code>");
  }
  dst += len;
  written += len;
  remaining -= len;
  (*current_line)++;

  while (*current_line < line_count) {
    const char *line = lines[*current_line];
    if (strcmp(line, "```") == 0) {
      (*current_line)++;
      break;
    }

    // Process escape sequences first
    char processed_line[1024];
    const char *src = line;
    char *processed_dst = processed_line;
    while (*src) {
      if (*src == '\\' && src[1] != '\0' && ispunct(src[1])) {
        *processed_dst++ = src[1];
        src += 2;
      } else {
        *processed_dst++ = *src++;
      }
    }
    *processed_dst = '\0';

    char escaped_line[2048];
    html_escape(processed_line, escaped_line, sizeof(escaped_line));
    len = snprintf(dst, remaining, "%s\n", escaped_line);
    dst += len;
    written += len;
    remaining -= len;
    (*current_line)++;
  }

  len = snprintf(dst, remaining, "</code></pre>");
  written += len;
  return written;
}

int
handle_indented_code(const char **lines, int line_count, int *current_line, char *output, size_t output_size)
{
  if (*current_line >= line_count) { return 0; }

  const char *first_line = lines[*current_line];
  if (strncmp(first_line, "    ", 4) != 0) { return 0; }

  char *dst = output;
  size_t remaining = output_size;
  int written = 0;

  int len = snprintf(dst, remaining, "<pre><code>");
  dst += len;
  written += len;
  remaining -= len;

  while (*current_line < line_count) {
    const char *line = lines[*current_line];
    if (strncmp(line, "    ", 4) != 0 && strlen(line) > 0) {
      break;
    }

    const char *content = strlen(line) >= 4 ? line + 4 : "";
    char escaped_line[2048];
    html_escape(content, escaped_line, sizeof(escaped_line));
    len = snprintf(dst, remaining, "%s\n", escaped_line);
    dst += len;
    written += len;
    remaining -= len;
    (*current_line)++;
  }

  len = snprintf(dst, remaining, "</code></pre>");
  written += len;
  return written;
}

int
handle_blockquote(const char **lines, int line_count, int *current_line, char *output, size_t output_size)
{
  if (*current_line >= line_count) { return 0; }

  const char *first_line = lines[*current_line];
  if (strncmp(first_line, "> ", 2) != 0) { return 0; }

  char *dst = output;
  size_t remaining = output_size;
  int written = 0;

  int len = snprintf(dst, remaining, "<blockquote><p>");
  dst += len;
  written += len;
  remaining -= len;

  while (*current_line < line_count) {
    const char *line = lines[*current_line];
    if (strncmp(line, "> ", 2) != 0) {
      break;
    }

    const char *content = line + 2;
    char *processed = process_inline_markup(content);
    len = snprintf(dst, remaining, "%s ", processed);
    dst += len;
    written += len;
    remaining -= len;
    (*current_line)++;
  }

  len = snprintf(dst, remaining, "</p></blockquote>");
  written += len;
  return written;
}

int
handle_paragraph(const char **lines, int line_count, int *current_line, char *output, size_t output_size)
{
  if (*current_line >= line_count) { return 0; }

  const char *line = lines[*current_line];
  if (strlen(line) == 0) {
    (*current_line)++;
    return 0;
  }

  // Collect paragraph content until blank line
  char paragraph_content[2048] = "";
  int first_line = 1;

  while (*current_line < line_count) {
    const char *current = lines[*current_line];
    
    // Check if line is blank (empty or whitespace only)
    const char *p = current;
    while (*p && isspace(*p)) p++;
    if (*p == '\0') {
      // Found blank line, stop collecting
      break;
    }

    // Add line to paragraph content
    if (first_line) {
      safe_strcpy(paragraph_content, current, sizeof(paragraph_content));
      first_line = 0;
    } else {
      // Add space between lines
      size_t current_len = strlen(paragraph_content);
      if (current_len < sizeof(paragraph_content) - 2) {
        paragraph_content[current_len] = ' ';
        paragraph_content[current_len + 1] = '\0';
        safe_strcpy(paragraph_content + current_len + 1, current, 
                   sizeof(paragraph_content) - current_len - 1);
      }
    }
    
    (*current_line)++;
  }

  if (strlen(paragraph_content) == 0) {
    return 0;
  }

  char *processed = process_inline_markup(paragraph_content);
  int written = snprintf(output, output_size, "<p>%s</p>", processed);
  return written > 0 ? written : 0;
}

static char *
process_markup(const char *text)
{
  static char output[MAX_MARKUP_OUTPUT];
  const char *lines[1024];
  int line_count = split_into_lines(text, lines, 1024);

  char *dst = output;
  size_t remaining = MAX_MARKUP_OUTPUT - 1;
  int current_line = 0;

  while (current_line < line_count && remaining > 1) {
    int matched = 0;
    const char *line = lines[current_line];

    // Check for footnote definitions first ([^...]: )
    if (strncmp(line, "[^", 2) == 0) {
      const char *bracket_end = strchr(line + 2, ']');
      if (bracket_end != NULL && bracket_end[1] == ':' && bracket_end[2] == ' ') {
        int written = handle_footnote_def(lines, line_count, &current_line, dst, remaining);
        if (written > 0) {
          dst += written;
          remaining -= written;
          matched = 1;
        }
      }
    }

    // Check for ordered lists (number followed by ". ")
    if (!matched && line[0] >= '1' && line[0] <= '9') {
      // Find where the number ends
      const char *ptr = line;
      while (*ptr >= '0' && *ptr <= '9') {
        ptr++;
      }
      // Check if it's immediately followed by ". "
      if (*ptr == '.' && ptr[1] == ' ') {
        int written = handle_list(lines, line_count, &current_line, dst, remaining);
        if (written > 0) {
          dst += written;
          remaining -= written;
          matched = 1;
        }
      }
    }

    if (!matched) {
      for (int i = 0; block_rules[i].prefix != NULL; i++) {
        if (strncmp(line, block_rules[i].prefix, block_rules[i].min_prefix_len) == 0) {
          int written = block_rules[i].handler(lines, line_count, &current_line, dst, remaining);
          if (written > 0) {
            dst += written;
            remaining -= written;
            matched = 1;
            break;
          }
        }
      }
    }

    if (!matched) {
      int written = handle_paragraph(lines, line_count, &current_line, dst, remaining);
      if (written > 0) {
        dst += written;
        remaining -= written;
      }
    }
  }

  *dst = '\0';
  return output;
}

static int
process_inline_markup_to_buffer_depth(const char *text, char *output, size_t output_size, int depth)
{
  // Prevent infinite recursion
  if (depth > 10) {
    size_t len = strlen(text);
    if (len >= output_size) len = output_size - 1;
    memcpy(output, text, len);
    output[len] = '\0';
    return len;
  }

  const char *src = text;
  char *dst = output;
  size_t remaining = output_size - 1;

  while (*src && remaining > 1) {
    if (*src == '\\' && src[1] != '\0' && ispunct(src[1])) {
      *dst++ = src[1];
      src += 2;
      remaining--;
      continue;
    }

    int matched = 0;
    for (int i = 0; inline_rules[i].prefix != NULL; i++) {
      if (strncmp(src, inline_rules[i].prefix, inline_rules[i].prefix_len) == 0) {
        const char *next_src;
        int written = inline_rules[i].handler(src, dst, remaining, &next_src);
        if (written > 0) {
          dst += written;
          remaining -= written;
          src = next_src;
          matched = 1;
          break;
        }
      }
    }

    if (!matched) {
      *dst++ = *src++;
      remaining--;
    }
  }

  *dst = '\0';
  return dst - output;
}

static int
process_inline_markup_to_buffer(const char *text, char *output, size_t output_size)
{
  return process_inline_markup_to_buffer_depth(text, output, output_size, 0);
}

char *
process_inline_markup(const char *text)
{
  static char output[MAX_MARKUP_OUTPUT];
  process_inline_markup_to_buffer(text, output, MAX_MARKUP_OUTPUT);
  return output;
}

int
handle_literal(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  (void)page; // unused
  (void)line_num; // unused for literals

  for (int i = 0; prefixes[i].prefix != NULL; i++) {
    if (strncmp(src, prefixes[i].prefix, prefixes[i].prefix_len) == 0 &&
        prefixes[i].handler == handle_literal) {

      const char *lit = prefixes[i].literal_output;
      size_t lit_len = strlen(lit);
      if (lit_len < remaining) {
        memcpy(dst, lit, lit_len);
        *next_src = src + prefixes[i].prefix_len;
        return lit_len;
      }
    }
  }
  return 0;
}

int
handle_variable(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  const char *var_start = src + 2;
  const char *var_end = strstr(var_start, "}}");
  if (var_end == NULL) {
    template_error(line_num, "Unclosed variable");
  }

  size_t var_len = var_end - var_start;
  char var_name[MAX_NAME_LEN];
  if (var_len >= MAX_NAME_LEN) {
    template_error(line_num, "Variable name too long");
  }

  memcpy(var_name, var_start, var_len);
  var_name[var_len] = '\0';
  char *trimmed_var = trim_whitespace(var_name);

  if (strlen(trimmed_var) == 0) {
    template_error(line_num, "Empty variable name in '{{}}'");
  }

  const char *value = get_variable_value(page, trimmed_var);
  char escaped_value[MAX_OUTPUT_LEN / 2];
  html_escape(value, escaped_value, sizeof(escaped_value));

  size_t value_len = strlen(escaped_value);
  if (value_len < remaining) {
    memcpy(dst, escaped_value, value_len);
    *next_src = var_end + 2;
    return value_len;
  }
  return 0;
}

int
handle_raw_variable(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  const char *var_start = src + 3;
  const char *var_end = strstr(var_start, "}}");
  if (var_end == NULL) {
    template_error(line_num, "Unclosed raw variable");
  }

  size_t var_len = var_end - var_start;
  char var_name[MAX_NAME_LEN];
  if (var_len >= MAX_NAME_LEN) {
    template_error(line_num, "Raw variable name too long");
  }

  memcpy(var_name, var_start, var_len);
  var_name[var_len] = '\0';
  char *trimmed_var = trim_whitespace(var_name);

  if (strlen(trimmed_var) == 0) {
    template_error(line_num, "Empty raw variable name in '{{&}}'");
  }

  const char *value = get_variable_value(page, trimmed_var);
  size_t value_len = strlen(value);
  if (value_len < remaining) {
    memcpy(dst, value, value_len);
    *next_src = var_end + 2;
    return value_len;
  }
  return 0;
}

int
handle_markup_variable(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  const char *var_start = src + 3;
  const char *var_end = strstr(var_start, "}}");
  if (var_end == NULL) {
    template_error(line_num, "Unclosed markup variable");
  }

  size_t var_len = var_end - var_start;
  char var_name[MAX_NAME_LEN];
  if (var_len >= MAX_NAME_LEN) {
    template_error(line_num, "Markup variable name too long");
  }

  memcpy(var_name, var_start, var_len);
  var_name[var_len] = '\0';
  char *trimmed_var = trim_whitespace(var_name);

  if (strlen(trimmed_var) == 0) {
    template_error(line_num, "Empty markup variable name in '{{!}}'");
  }

  const char *value = get_variable_value(page, trimmed_var);
  char *processed = process_markup(value);

  size_t value_len = strlen(processed);
  if (value_len < remaining) {
    memcpy(dst, processed, value_len);
    *next_src = var_end + 2;
    return value_len;
  }
  return 0;
}

int
handle_file_include(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  (void)page; // unused

  const char *path_start = src + 3;
  const char *path_end = strstr(path_start, "}}");
  if (path_end == NULL) {
    template_error(line_num, "Unclosed file include");
  }

  size_t path_len = path_end - path_start;
  char file_path[MAX_PATH_LEN];
  if (path_len >= MAX_PATH_LEN) {
    template_error(line_num, "File path too long");
  }

  memcpy(file_path, path_start, path_len);
  file_path[path_len] = '\0';
  char *trimmed_path = trim_whitespace(file_path);

  if (strlen(trimmed_path) == 0) {
    template_error(line_num, "Empty file path in '{{<}}'");
  }

  FILE *file = fopen(trimmed_path, "r");
  if (file == NULL) {
    template_error(line_num, "Cannot open included file");
  }

  char file_content[MAX_CONTENT_LEN];
  size_t bytes_read = fread(file_content, 1, sizeof(file_content) - 1, file);
  file_content[bytes_read] = '\0';
  fclose(file);

  size_t content_len = strlen(file_content);
  if (content_len < remaining) {
    memcpy(dst, file_content, content_len);
    *next_src = path_end + 2;
    return content_len;
  }

  return 0;
}

int
handle_file_include_escaped(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  (void)page; // unused

  const char *path_start = src + 3;
  const char *path_end = strstr(path_start, "}}");
  if (path_end == NULL) {
    template_error(line_num, "Unclosed escaped file include");
  }

  size_t path_len = path_end - path_start;
  char file_path[MAX_PATH_LEN];
  if (path_len >= MAX_PATH_LEN) {
    template_error(line_num, "File path too long");
  }

  memcpy(file_path, path_start, path_len);
  file_path[path_len] = '\0';
  char *trimmed_path = trim_whitespace(file_path);

  if (strlen(trimmed_path) == 0) {
    template_error(line_num, "Empty file path in '{{^}}'");
  }

  FILE *file = fopen(trimmed_path, "r");
  if (file == NULL) {
    template_error(line_num, "Cannot open included file");
  }

  char file_content[MAX_CONTENT_LEN];
  size_t bytes_read = fread(file_content, 1, sizeof(file_content) - 1, file);
  file_content[bytes_read] = '\0';
  fclose(file);

  char escaped_content[MAX_CONTENT_LEN * 2];
  html_escape(file_content, escaped_content, sizeof(escaped_content));

  size_t content_len = strlen(escaped_content);
  if (content_len < remaining) {
    memcpy(dst, escaped_content, content_len);
    *next_src = path_end + 2;
    return content_len;
  }

  return 0;
}

int
handle_iteration_block(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  const char *var_start = src + 3;
  const char *var_end = strstr(var_start, "}}");
  if (var_end == NULL) {
    template_error(line_num, "Unclosed iteration block");
  }

  size_t var_len = var_end - var_start;
  char var_name[MAX_NAME_LEN];
  if (var_len >= MAX_NAME_LEN) {
    template_error(line_num, "Iteration block name too long");
  }

  memcpy(var_name, var_start, var_len);
  var_name[var_len] = '\0';
  char *trimmed_var = trim_whitespace(var_name);

  if (strlen(trimmed_var) == 0) {
    template_error(line_num, "Empty iteration block name in '{{#}}'");
  }

  char end_tag[64];
  snprintf(end_tag, sizeof(end_tag), "{{/%s}}", trimmed_var);

  const char *block_start = var_end + 2;
  const char *block_end = strstr(block_start, end_tag);
  if (block_end == NULL) {
    template_error(line_num, "Missing end tag for iteration block");
  }

  // Validate known iteration types
  if (strcmp(trimmed_var, "pages") != 0 &&
      strcmp(trimmed_var, "BACKLINKS") != 0 &&
      strcmp(trimmed_var, "SIBLINGS") != 0) {
    template_error(line_num, "Unknown iteration block type");
  }

  if (strcmp(trimmed_var, "pages") == 0) {
    char *output_pos = dst;
    size_t remaining_space = remaining;

    struct page *saved_current = iter_context.current_page;
    iter_context.root_page = page;

    for (int j = 0; j < current_page_count; j++) {
      iter_context.current_page = &current_pages[j];

      char block_content[MAX_OUTPUT_LEN];
      size_t block_len = block_end - block_start;
      if (block_len >= MAX_OUTPUT_LEN) break;

      memcpy(block_content, block_start, block_len);
      block_content[block_len] = '\0';

      char *rendered_block = render_template_block(page, block_content);
      size_t rendered_len = strlen(rendered_block);
      if (rendered_len < remaining_space) {
        memcpy(output_pos, rendered_block, rendered_len);
        output_pos += rendered_len;
        remaining_space -= rendered_len;
      }
      free(rendered_block);
    }

    iter_context.current_page = saved_current;
    *next_src = block_end + strlen(end_tag);
    return output_pos - dst;
  }
  else if (strcmp(trimmed_var, "BACKLINKS") == 0) {
    char *output_pos = dst;
    size_t remaining_space = remaining;

    struct page *saved_current = iter_context.current_page;
    struct page *target_page = iter_context.current_page ? iter_context.current_page : page;

    for (int j = 0; j < target_page->backlink_count; j++) {
      int backlink_idx = target_page->backlinks[j];
      if (backlink_idx < 0 || backlink_idx >= page_count) {
        panic("Invalid backlink index");
      }
      iter_context.current_page = &current_pages[backlink_idx];

      char block_content[MAX_OUTPUT_LEN];
      size_t block_len = block_end - block_start;
      if (block_len >= MAX_OUTPUT_LEN) break;

      memcpy(block_content, block_start, block_len);
      block_content[block_len] = '\0';

      char *rendered_block = render_template_block(page, block_content);
      size_t rendered_len = strlen(rendered_block);
      if (rendered_len < remaining_space) {
        memcpy(output_pos, rendered_block, rendered_len);
        output_pos += rendered_len;
        remaining_space -= rendered_len;
      }
      free(rendered_block);
    }

    iter_context.current_page = saved_current;
    *next_src = block_end + strlen(end_tag);
    return output_pos - dst;
  }
  else if (strcmp(trimmed_var, "SIBLINGS") == 0) {
    char *output_pos = dst;
    size_t remaining_space = remaining;

    struct page *saved_current = iter_context.current_page;
    struct page *target_page = iter_context.current_page ? iter_context.current_page : page;

    for (int j = 0; j < target_page->sibling_count; j++) {
      int sibling_idx = target_page->siblings[j];
      if (sibling_idx < 0 || sibling_idx >= page_count) {
        panic("Invalid sibling index");
      }
      iter_context.current_page = &current_pages[sibling_idx];

      char block_content[MAX_OUTPUT_LEN];
      size_t block_len = block_end - block_start;
      if (block_len >= MAX_OUTPUT_LEN) break;

      memcpy(block_content, block_start, block_len);
      block_content[block_len] = '\0';

      char *rendered_block = render_template_block(page, block_content);
      size_t rendered_len = strlen(rendered_block);
      if (rendered_len < remaining_space) {
        memcpy(output_pos, rendered_block, rendered_len);
        output_pos += rendered_len;
        remaining_space -= rendered_len;
      }
      free(rendered_block);
    }

    iter_context.current_page = saved_current;
    *next_src = block_end + strlen(end_tag);
    return output_pos - dst;
  }

  return 0;
}

int
handle_conditional_block(struct page *page, const char *src, char *dst, size_t remaining, const char **next_src, int line_num)
{
  const char *var_start = src + 3;
  const char *var_end = strstr(var_start, "}}");
  if (var_end == NULL) {
    template_error(line_num, "Unclosed conditional block");
  }

  size_t var_len = var_end - var_start;
  char var_name[MAX_NAME_LEN];
  if (var_len >= MAX_NAME_LEN) {
    template_error(line_num, "Conditional block name too long");
  }

  memcpy(var_name, var_start, var_len);
  var_name[var_len] = '\0';
  
  // Trim whitespace from variable name
  char *var_ptr = var_name;
  while (*var_ptr == ' ' || *var_ptr == '\t') var_ptr++;
  char *var_end_ptr = var_ptr + strlen(var_ptr) - 1;
  while (var_end_ptr > var_ptr && (*var_end_ptr == ' ' || *var_end_ptr == '\t')) {
    *var_end_ptr = '\0';
    var_end_ptr--;
  }
  char trimmed_var[MAX_NAME_LEN];
  safe_strcpy(trimmed_var, var_ptr, MAX_NAME_LEN);

  // Find the closing block
  char end_tag[64];
  snprintf(end_tag, sizeof(end_tag), "{{/%s}}", trimmed_var);
  
  const char *block_start = var_end + 2;
  const char *block_end = strstr(block_start, end_tag);
  if (block_end == NULL) {
    template_error(line_num, "Unclosed conditional block");
  }

  // Only support pages conditional for now
  if (strcmp(trimmed_var, "pages") != 0) {
    template_error(line_num, "Unknown conditional block type");
  }

  // Render the block only if there are NO pages
  if (current_page_count == 0) {
    char block_content[MAX_OUTPUT_LEN];
    size_t block_len = block_end - block_start;
    if (block_len >= MAX_OUTPUT_LEN) {
      template_error(line_num, "Conditional block content too long");
    }

    memcpy(block_content, block_start, block_len);
    block_content[block_len] = '\0';

    char *rendered_block = render_template_block(page, block_content);
    size_t rendered_len = strlen(rendered_block);
    if (rendered_len < remaining) {
      memcpy(dst, rendered_block, rendered_len);
      *next_src = block_end + strlen(end_tag);
      free(rendered_block);
      return rendered_len;
    }
    free(rendered_block);
  } else {
    // Skip the block if there are pages
    *next_src = block_end + strlen(end_tag);
    return 0;
  }

  return 0;
}

const char *
get_variable_value(struct page *page, const char *var_name)
{
  if (page == NULL) {
    panic("NULL page pointer in get_variable_value");
  }
  if (var_name == NULL) {
    panic("NULL var_name pointer in get_variable_value");
  }

  // Check for time variables
  if (strcmp(var_name, "NOW") == 0) {
    static char now_buffer[32];
    time_t now = time(NULL);
    struct tm *local_tm = localtime(&now);
    strftime(now_buffer, sizeof(now_buffer), "%Y-%m-%dT%H:%M:%S%z", local_tm);
    return now_buffer;
  }
  
  if (strcmp(var_name, "UTCNOW") == 0) {
    static char utcnow_buffer[32];
    time_t now = time(NULL);
    struct tm *utc_tm = gmtime(&now);
    strftime(utcnow_buffer, sizeof(utcnow_buffer), "%Y-%m-%dT%H:%M:%SZ", utc_tm);
    return utcnow_buffer;
  }

  // Check for parent. prefix to access parent page variables
  if (strncmp(var_name, "parent.", 7) == 0) {
    const char *parent_var_name = var_name + 7;
    struct page *parent_page = iter_context.root_page ? iter_context.root_page : page;
    
    if (strcmp(parent_var_name, "content") == 0) {
      return parent_page->content;
    }
    
    if (strcmp(parent_var_name, "name") == 0) {
      return parent_page->name;
    }
    
    const char *meta = get_metadata(parent_page, parent_var_name);
    return meta ? meta : "";
  }

  struct page *target_page = iter_context.current_page ? iter_context.current_page : page;

  if (strcmp(var_name, "content") == 0) {
    return target_page->content;
  }

  if (strcmp(var_name, "name") == 0) {
    return target_page->name;
  }

  const char *meta = get_metadata(target_page, var_name);
  return meta ? meta : "";
}

char *
render_template_block(struct page *page, const char *template_text)
{
  char *output = malloc(MAX_OUTPUT_LEN);
  if (!output) {
    panic("Failed to allocate memory for template output");
  }

  const char *src = template_text;
  char *dst = output;
  size_t remaining = MAX_OUTPUT_LEN - 1;
  int line_num = 1;

  while (*src && remaining > 1) {
    int matched = 0;

    for (int i = 0; prefixes[i].prefix != NULL; i++) {
      if (strncmp(src, prefixes[i].prefix, prefixes[i].prefix_len) == 0) {
        const char *next_src;
        int written = prefixes[i].handler(page, src, dst, remaining, &next_src, line_num);
        if (written >= 0 && next_src > src) {
          dst += written;
          remaining -= written;
          src = next_src;
          matched = 1;
          break;
        }
      }
    }

    if (!matched) {
      if (*src == '\n') {
        line_num++;
      }
      *dst++ = *src++;
      remaining--;
    }
  }

  *dst = '\0';
  return output;
}

static char *
render_template(struct page *page)
{
  static char final_output[MAX_OUTPUT_LEN];

  iter_context.current_page = NULL;
  iter_context.root_page = page;

  char *dynamic_output = render_template_block(page, template_content);
  safe_strcpy(final_output, dynamic_output, MAX_OUTPUT_LEN);
  free(dynamic_output);

  return final_output;
}

static void
extract_name_from_filename(const char *filename, char *name, size_t name_len)
{
  const char *basename = strrchr(filename, '/');
  if (basename == NULL) {
    basename = filename;
  }
  else {
    basename++;
  }

  safe_strcpy(name, basename, name_len);

  char *dot = strrchr(name, '.');
  if (dot != NULL) {
    *dot = '\0';
  }
}

static int
has_extension(const char *filename, const char *ext)
{
  size_t name_len = strlen(filename);
  size_t ext_len = strlen(ext);

  if (name_len < ext_len) { return 0; }
  return strcmp(filename + name_len - ext_len, ext) == 0;
}

static void
scan_directory(const char *dir_path, const char *base_path, struct file_info *files, int *file_count)
{
  DIR *dir = opendir(dir_path);
  if (!dir) { return; }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL && *file_count < MAX_FILES) {
    if (entry->d_name[0] == '.') continue;

    char full_path[MAX_PATH_LEN];

    if (strlen(dir_path) + strlen(entry->d_name) + 2 > MAX_PATH_LEN) {
      panic("Path too long");
    }

    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

    struct stat st;
    if (stat(full_path, &st) != 0) continue;

    if (S_ISDIR(st.st_mode)) {
      scan_directory(full_path, base_path, files, file_count);
    }
    else if (S_ISREG(st.st_mode) && has_extension(entry->d_name, FOLDR_DATA_EXT)) {
      safe_strcpy(files[*file_count].path, full_path, MAX_PATH_LEN);

      const char *rel_start = full_path;
      if (strncmp(full_path, base_path, strlen(base_path)) == 0) {
        rel_start = full_path + strlen(base_path);
        while (*rel_start == '/') rel_start++;
      }
      safe_strcpy(files[*file_count].relative_path, rel_start, MAX_PATH_LEN);

      (*file_count)++;
    }
  }

  closedir(dir);
}

static int
collect_files(int argc, char *argv[], int start_index, struct file_info *files)
{
  int file_count = 0;

  for (int i = start_index; i < argc && file_count < MAX_FILES; i++) {
    struct stat st;
    if (stat(argv[i], &st) != 0) {
      char msg[512];
      snprintf(msg, sizeof(msg), "Cannot access: %s", argv[i]);
      panic(msg);
    }

    if (S_ISDIR(st.st_mode)) {
      scan_directory(argv[i], argv[i], files, &file_count);
    }
    else if (S_ISREG(st.st_mode)) {
      // Accept both .fmd files and any other file (for backwards compatibility)
      safe_strcpy(files[file_count].path, argv[i], MAX_PATH_LEN);

      const char *basename = strrchr(argv[i], '/');
      if (basename) {
        basename++;
      }
      else {
        basename = argv[i];
      }
      safe_strcpy(files[file_count].relative_path, basename, MAX_PATH_LEN);

      file_count++;
    }
  }

  return file_count;
}

static void
derive_page_name(const char *filepath, const char *base_dir, char *page_name, size_t page_name_len)
{
  const char *relative_start = filepath;

  // Skip base_dir prefix if it exists
  if (base_dir && strncmp(filepath, base_dir, strlen(base_dir)) == 0) {
    relative_start = filepath + strlen(base_dir);
    while (*relative_start == '/') relative_start++; // Skip leading slashes
  }

  // Remove file extension
  const char *last_dot = strrchr(relative_start, '.');
  if (last_dot) {
    int base_len = last_dot - relative_start;
    snprintf(page_name, page_name_len, "%.*s", base_len, relative_start);
  }
  else {
    snprintf(page_name, page_name_len, "%s", relative_start);
  }
}

static void
parse_file_with_base(const char *filename, const char *base_dir)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    char msg[512];
    snprintf(msg, sizeof(msg), "Cannot open file: %s", filename);
    panic(msg);
  }

  if (page_count >= MAX_PAGES) {
    panic("Too many pages");
  }

  struct page *page = &pages[page_count++];
  memset(page, 0, sizeof(struct page));

  derive_page_name(filename, base_dir, page->name, MAX_NAME_LEN);
  safe_strcpy(page->source_path, filename, MAX_PATH_LEN);

  char buffer[MAX_CONTENT_LEN + MAX_METADATA_LEN];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
  buffer[bytes_read] = '\0';
  fclose(file);

  if (strncmp(buffer, "---\n", 4) == 0) {
    parse_front_matter(page, buffer);

    const char *content_start = strstr(buffer + 4, "\n---\n");
    if (content_start != NULL) {
      content_start += 5;
      safe_strcpy(page->content, content_start, MAX_CONTENT_LEN);
    }
  }
  else {
    safe_strcpy(page->content, buffer, MAX_CONTENT_LEN);
  }
}

static void
parse_file(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    char msg[512];
    snprintf(msg, sizeof(msg), "Cannot open file: %s", filename);
    panic(msg);
  }

  if (page_count >= MAX_PAGES) {
    panic("Too many pages");
  }

  struct page *page = &pages[page_count++];
  memset(page, 0, sizeof(struct page));

  extract_name_from_filename(filename, page->name, MAX_NAME_LEN);
  safe_strcpy(page->source_path, filename, MAX_PATH_LEN);

  char buffer[MAX_CONTENT_LEN + MAX_METADATA_LEN];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
  buffer[bytes_read] = '\0';
  fclose(file);

  if (strncmp(buffer, "---\n", 4) == 0) {
    parse_front_matter(page, buffer);

    const char *content_start = strstr(buffer + 4, "\n---\n");
    if (content_start != NULL) {
      content_start += 5;
      safe_strcpy(page->content, content_start, MAX_CONTENT_LEN);
    }
  }
  else {
    safe_strcpy(page->content, buffer, MAX_CONTENT_LEN);
  }
}

static void
create_directory_recursive(const char *path)
{
  char temp[MAX_PATH_LEN];
  char *p = temp;
  snprintf(temp, sizeof(temp), "%s", path);

  while (*p) {
    if (*p == '/' && p != temp) {
      *p = '\0';
      if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
        panic_with_path("Failed to create directory:", temp);
      }
      *p = '/';
    }
    p++;
  }
  if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
    panic_with_path("Failed to create directory:", temp);
  }
}

static void
derive_output_path(const char *source_path, const char *output_dir, const char *base_dir, char *output_path, size_t output_path_len)
{
  const char *relative_start = source_path;

  // Extract base directory name and preserve it in output path
  static char base_dir_name_buffer[MAX_PATH_LEN];
  const char *base_dir_name = NULL;
  if (base_dir) {
    // Make a copy to work with, removing trailing slashes
    char base_dir_copy[MAX_PATH_LEN];
    strncpy(base_dir_copy, base_dir, sizeof(base_dir_copy) - 1);
    base_dir_copy[sizeof(base_dir_copy) - 1] = '\0';
    
    // Remove trailing slashes
    int len = strlen(base_dir_copy);
    while (len > 1 && base_dir_copy[len - 1] == '/') {
      base_dir_copy[len - 1] = '\0';
      len--;
    }
    
    // Find the last component of base_dir
    const char *last_slash = strrchr(base_dir_copy, '/');
    if (last_slash) {
      safe_strcpy(base_dir_name_buffer, last_slash + 1, MAX_PATH_LEN);
      base_dir_name = base_dir_name_buffer;
    } else if (strlen(base_dir_copy) > 0) {
      // No slash found, entire string is the directory name (unless it's just ".")
      if (strcmp(base_dir_copy, ".") != 0) {
        safe_strcpy(base_dir_name_buffer, base_dir_copy, MAX_PATH_LEN);
        base_dir_name = base_dir_name_buffer;
      }
    }
    
    // Skip base_dir prefix if it exists
    if (strncmp(source_path, base_dir, strlen(base_dir)) == 0) {
      relative_start = source_path + strlen(base_dir);
      while (*relative_start == '/') relative_start++; // Skip leading slashes
    }
  }

  // Find the last dot to replace extension
  const char *last_dot = strrchr(relative_start, '.');
  const char *last_slash = strrchr(relative_start, '/');

  if (output_dir) {
    if (last_dot && (!last_slash || last_dot > last_slash)) {
      // Replace extension with .html
      int base_len = last_dot - relative_start;
      if (base_dir_name) {
        // Include base directory name in output path
        if (strlen(output_dir) + strlen(base_dir_name) + base_len + 7 >= output_path_len) { // +7 for "//.html\0"
          panic("Output path would be too long");
        }
        if (strlen(relative_start) > 0 && relative_start[0] != '/') {
          snprintf(output_path, output_path_len, "%s/%s/%.*s.html", output_dir, base_dir_name, base_len, relative_start);
        } else {
          snprintf(output_path, output_path_len, "%s/%s%.*s.html", output_dir, base_dir_name, base_len, relative_start);
        }
      } else {
        if (strlen(output_dir) + base_len + 6 >= output_path_len) { // +6 for "/.html\0"
          panic("Output path would be too long");
        }
        snprintf(output_path, output_path_len, "%s/%.*s.html", output_dir, base_len, relative_start);
      }
    }
    else {
      // No extension found, just add .html
      if (base_dir_name) {
        // Include base directory name in output path
        if (strlen(output_dir) + strlen(base_dir_name) + strlen(relative_start) + 7 >= output_path_len) { // +7 for "//.html\0"
          panic("Output path would be too long");
        }
        if (strlen(relative_start) > 0 && relative_start[0] != '/') {
          snprintf(output_path, output_path_len, "%s/%s/%s.html", output_dir, base_dir_name, relative_start);
        } else {
          snprintf(output_path, output_path_len, "%s/%s%s.html", output_dir, base_dir_name, relative_start);
        }
      } else {
        if (strlen(output_dir) + strlen(relative_start) + 6 >= output_path_len) { // +6 for "/.html\0"
          panic("Output path would be too long");
        }
        snprintf(output_path, output_path_len, "%s/%s.html", output_dir, relative_start);
      }
    }
  }
  else {
    // No output directory, just change extension
    if (last_dot && (!last_slash || last_dot > last_slash)) {
      int base_len = last_dot - source_path;
      if ((size_t)base_len + 5 >= output_path_len) { // +5 for ".html\0"
        panic("Output path would be too long");
      }
      snprintf(output_path, output_path_len, "%.*s.html", base_len, source_path);
    }
    else {
      if (strlen(source_path) + 5 >= output_path_len) { // +5 for ".html\0"
        panic("Output path would be too long");
      }
      snprintf(output_path, output_path_len, "%s.html", source_path);
    }
  }
}

static void
render_page_to_file(struct page *page, const char *template_file, const char *output_dir, const char *base_dir)
{
  if (template_file != NULL) {
    load_template(template_file);
    char *rendered = render_template(page);

    char output_path[MAX_PATH_LEN];
    derive_output_path(page->source_path, output_dir, base_dir, output_path, sizeof(output_path));

    // Create directory structure if needed
    if (output_dir) {
      char dir_path[MAX_PATH_LEN];
      snprintf(dir_path, sizeof(dir_path), "%s", output_path);
      char *last_slash = strrchr(dir_path, '/');
      if (last_slash && last_slash != dir_path) {
        *last_slash = '\0';
        create_directory_recursive(dir_path);
      }
    }

    FILE *output_file = fopen(output_path, "w");
    if (output_file == NULL) {
      char msg[512];
      snprintf(msg, sizeof(msg), "Cannot create output file: %s", output_path);
      panic(msg);
    }

    fprintf(output_file, "%s", rendered);
    fclose(output_file);
    printf("Generated: %s\n", output_path);
  }
}

static int
usage(char *argv0)
{
  fprintf(stderr, "usage: %s [-t template.html.tpl] [-o output_dir] [-f key:value] [--order-by key:direction] [--limit number] <file1> [file2] ...\n", argv0);
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -t, --template FILE    Use template file\n");
  fprintf(stderr, "  -o, --output DIR       Output directory\n");
  fprintf(stderr, "  -f, --filter KEY:VAL   Filter pages by metadata\n");
  fprintf(stderr, "  --order-by KEY:DIR     Sort by metadata key (DIR: asc|desc)\n");
  fprintf(stderr, "  --limit NUMBER         Limit number of pages processed\n");
  return 1;
}

int
main(int argc, char *argv[])
{
  const char *template_file = NULL;
  const char *output_dir = NULL;
  struct filter filters[MAX_FILTERS];
  int filter_count = 0;
  int limit = -1;  // -1 means no limit
  char order_by_key[MAX_NAME_LEN] = "";
  int order_ascending = 1;  // 1 for asc, 0 for desc
  int arg_idx = 1;

  if (argc < 2) {
    return usage(argv[0]);
  }

  // Parse command line options
  while (arg_idx < argc && argv[arg_idx][0] == '-') {
    if (strcmp(argv[arg_idx], "-t") == 0 || strcmp(argv[arg_idx], "--template") == 0) {
      if (arg_idx + 1 >= argc) {
        fprintf(stderr, "error: -t requires a template file\n");
        return 1;
      }
      template_file = argv[arg_idx + 1];
      arg_idx += 2;
    }
    else if (strcmp(argv[arg_idx], "-o") == 0 || strcmp(argv[arg_idx], "--output") == 0) {
      if (arg_idx + 1 >= argc) {
        fprintf(stderr, "error: -o/--output requires a directory\n");
        return 1;
      }
      output_dir = argv[arg_idx + 1];
      arg_idx += 2;
    }
    else if (strcmp(argv[arg_idx], "-f") == 0 || strcmp(argv[arg_idx], "--filter") == 0) {
      if (arg_idx + 1 >= argc) {
        fprintf(stderr, "error: -f/--filter requires a key:value pair\n");
        return 1;
      }
      if (filter_count >= MAX_FILTERS) {
        fprintf(stderr, "error: Too many filters (max %d)\n", MAX_FILTERS);
        return 1;
      }
      if (!parse_filter(argv[arg_idx + 1], &filters[filter_count])) {
        fprintf(stderr, "error: Invalid filter format '%s' (expected key:value)\n", argv[arg_idx + 1]);
        return 1;
      }
      filter_count++;
      arg_idx += 2;
    }
    else if (strcmp(argv[arg_idx], "--limit") == 0) {
      if (arg_idx + 1 >= argc) {
        fprintf(stderr, "error: --limit requires a number\n");
        return 1;
      }
      limit = atoi(argv[arg_idx + 1]);
      if (limit <= 0) {
        fprintf(stderr, "error: --limit must be a positive number\n");
        return 1;
      }
      arg_idx += 2;
    }
    else if (strcmp(argv[arg_idx], "--order-by") == 0) {
      if (arg_idx + 1 >= argc) {
        fprintf(stderr, "error: --order-by requires key:direction (e.g., date:desc)\n");
        return 1;
      }
      const char *order_spec = argv[arg_idx + 1];
      const char *colon = strchr(order_spec, ':');
      if (!colon) {
        fprintf(stderr, "error: --order-by format must be key:direction (e.g., date:desc)\n");
        return 1;
      }
      
      size_t key_len = colon - order_spec;
      if (key_len >= MAX_NAME_LEN) {
        fprintf(stderr, "error: order-by key too long\n");
        return 1;
      }
      
      strncpy(order_by_key, order_spec, key_len);
      order_by_key[key_len] = '\0';
      
      const char *direction = colon + 1;
      if (strcmp(direction, "asc") == 0) {
        order_ascending = 1;
      } else if (strcmp(direction, "desc") == 0) {
        order_ascending = 0;
      } else {
        fprintf(stderr, "error: order direction must be 'asc' or 'desc'\n");
        return 1;
      }
      arg_idx += 2;
    }
    else {
      fprintf(stderr, "error: unknown option: %s\n", argv[arg_idx]);
      return 1;
    }
  }

  if (arg_idx >= argc) {
    return usage(argv[0]);
  }

  // Collect all files to process
  static struct file_info files[MAX_FILES];
  int file_count = collect_files(argc, argv, arg_idx, files);

  // Determine base directory (first directory argument or current directory)
  const char *base_dir = NULL;
  for (int i = arg_idx; i < argc; i++) {
    struct stat st;
    if (stat(argv[i], &st) == 0 && S_ISDIR(st.st_mode)) {
      base_dir = argv[i];
      break;
    }
  }

  // Parse all collected files
  for (int i = 0; i < file_count; i++) {
    parse_file_with_base(files[i].path, base_dir);
  }

  // Save the original first page before sorting (for single output mode context)
  struct page original_first_page;
  if (page_count > 0) {
    original_first_page = pages[0];
  }

  // Apply custom sorting if specified
  if (strlen(order_by_key) > 0) {
    // Set global variables for the comparison function
    safe_strcpy(sort_key, order_by_key, MAX_NAME_LEN);
    sort_ascending = order_ascending;
    qsort(pages, page_count, sizeof(struct page), compare_pages_by_metadata);
  } else {
    qsort(pages, page_count, sizeof(struct page), compare_pages_by_name);
  }

  build_category_tree();
  build_backlinks();
  
  // Initialize current pages to point to main pages array
  current_pages = pages;
  current_page_count = page_count;

  if (template_file != NULL) {
    // Check if output_dir is actually a file (single output mode)
    int single_output_mode = 0;
    if (output_dir != NULL) {
      // If output_dir has an extension, treat as single file
      const char *last_slash = strrchr(output_dir, '/');
      const char *last_dot = strrchr(output_dir, '.');
      if (last_dot && (!last_slash || last_dot > last_slash)) {
        single_output_mode = 1;
      }
    }
    
    if (single_output_mode && page_count > 0) {
      // Single output mode: original first page is context, rest are filtered pages collection
      struct page *context_page = &original_first_page;
      
      // Create filtered pages array excluding the original first page (context)
      static struct page filtered_pages[MAX_PAGES];
      int filtered_count = 0;
      
      for (int i = 0; i < page_count && filtered_count < MAX_PAGES; i++) {
        // Skip the original first page (it's our context page)
        if (strcmp(pages[i].name, original_first_page.name) == 0 &&
            strcmp(pages[i].source_path, original_first_page.source_path) == 0) {
          continue;
        }
        
        // Check if we've hit the limit
        if (limit > 0 && filtered_count >= limit) {
          break;
        }
        
        if (filter_count == 0 || page_matches_filters(&pages[i], filters, filter_count)) {
          filtered_pages[filtered_count] = pages[i];
          filtered_count++;
        }
      }
      
      // Set current pages to filtered set for template rendering
      current_pages = filtered_pages;
      current_page_count = filtered_count;
      
      // Render the context page
      load_template(template_file);
      char *rendered = render_template(context_page);
      
      // Restore current pages to original set
      current_pages = pages;
      current_page_count = page_count;
      
      if (rendered != NULL) {
        // Create directory if needed
        char dir_path[MAX_PATH_LEN];
        safe_strcpy(dir_path, output_dir, MAX_PATH_LEN);
        char *last_slash = strrchr(dir_path, '/');
        if (last_slash && last_slash != dir_path) {
          *last_slash = '\0';
          create_directory_recursive(dir_path);
        }
        
        FILE *output_file = fopen(output_dir, "w");
        if (output_file == NULL) {
          char msg[512];
          snprintf(msg, sizeof(msg), "Cannot create output file: %s", output_dir);
          panic(msg);
        }
        
        fprintf(output_file, "%s", rendered);
        fclose(output_file);
        printf("Generated: %s\n", output_dir);
      }
    }
    else {
      // Multi-output mode: render each page individually (existing behavior)
      int rendered_count = 0;
      for (int i = 0; i < page_count; i++) {
        // Check if we've hit the limit
        if (limit > 0 && rendered_count >= limit) {
          break;
        }
        
        if (filter_count == 0 || page_matches_filters(&pages[i], filters, filter_count)) {
          render_page_to_file(&pages[i], template_file, output_dir, base_dir);
          rendered_count++;
        }
      }
    }
  }
  else {
    print_page_structure();
  }

  return 0;
}

int handle_footnote_def(const char **lines, int line_count, int *current_line, char *output, size_t output_size) {
  if (*current_line >= line_count) { return 0; }

  const char *line = lines[*current_line];
  if (strncmp(line, "[^", 2) != 0) { return 0; }

  const char *ref_start = line + 2;
  const char *bracket_end = strchr(ref_start, ']');
  if (bracket_end == NULL || bracket_end[1] != ':' || bracket_end[2] != ' ') { return 0; }

  size_t ref_len = bracket_end - ref_start;
  const char *content_start = bracket_end + 3;  // Skip "]: "

  // Process escape sequences in the reference
  char processed_ref[64];
  char ref_buf[64];
  snprintf(ref_buf, sizeof(ref_buf), "%.*s", (int)ref_len, ref_start);

  const char *ref_src = ref_buf;
  char *ref_dst = processed_ref;
  while (*ref_src) {
    if (*ref_src == '\\' && ref_src[1] != '\0' && ispunct(ref_src[1])) {
      *ref_dst++ = ref_src[1];
      ref_src += 2;
    } else {
      *ref_dst++ = *ref_src++;
    }
  }
  *ref_dst = '\0';

  // Simple escape processing for footnote content
  char processed_content[1024];
  const char *content_src = content_start;
  char *content_dst = processed_content;
  while (*content_src) {
    if (*content_src == '\\' && content_src[1] != '\0' && ispunct(content_src[1])) {
      *content_dst++ = content_src[1];
      content_src += 2;
    } else {
      *content_dst++ = *content_src++;
    }
  }
  *content_dst = '\0';

  int written = snprintf(output, output_size,
                        "<div id=\"fn%s\" class=\"footnote\">%s <a href=\"#ref-%s\">↩</a></div>",
                        processed_ref, processed_content, processed_ref);

  (*current_line)++;
  return written > 0 ? written : 0;
}
