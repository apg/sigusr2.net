FOLDR = ../foldr
BUILD_DIR = ../build

SRCS_MK = sources.mk
FONTS_MK = fonts.mk

-include $(SRCS_MK)
-include $(FONTS_MK)

all: site

$(SRCS_MK):
	@find . -name '*.fmd' -type f 2>/dev/null | \
    awk 'BEGIN{printf "SRCS ="} {printf " \"%s\"", $$0} END{print ""}' > $(SRCS_MK)

$(FONTS_MK):
	@find ./f -name '*.woff*' -type f 2>/dev/null | \
    awk 'BEGIN{printf "FONTS ="} {printf " \"%s\"", $$0} END{print ""}' > $(FONTS_MK)


site: home posts posts-index pages essays comics i f feed.xml favicon

.PHONY: favicon
favicon:
	@echo "Copying favicon to build dir..."
	@mkdir -p $(BUILD_DIR)
	@cp favicon.png $(BUILD_DIR)/favicon.png

.PHONY: i
i:
	@echo "Copying images to build dir..."
	@mkdir -p $(BUILD_DIR)
	@cp -R ./i $(BUILD_DIR)/i

f: $(FONTS_MK)
	@echo "Copying fonts to build dir..."
	@mkdir -p $(BUILD_DIR)/f
	@cp $(FONTS) $(BUILD_DIR)/f

# build the atom feed.
feed.xml: templates/atom.xml.tpl $(SRCS_MK)
	$(FOLDR) -t templates/atom.xml.tpl \
			-o $(BUILD_DIR)/feed.xml \
			-f blog:true --limit 10 \
			--order-by publish_date:desc \
			feed.fmd .

home: templates/home.html.tpl $(SRCS_MK)
	$(FOLDR) -t templates/home.html.tpl \
			-o $(BUILD_DIR)/index.html \
			-f blog:true --limit 10 \
			--order-by publish_date:desc \
			index.fmd .

posts-index: templates/posts-index.html.tpl $(SRCS_MK)
	$(FOLDR) -t templates/posts-index.html.tpl \
			-o $(BUILD_DIR)/posts.html \
			-f blog:true \
			--order-by publish_date:desc \
			posts.fmd .

posts: templates/post.html.tpl $(SRCS_MK)
	$(FOLDR) -t templates/post.html.tpl \
		-o $(BUILD_DIR) \
		-f blog:true \
		.

pages: templates/page.html.tpl $(SRCS_MK)
	$(FOLDR) -t templates/page.html.tpl \
		-o $(BUILD_DIR) \
		-f page:true \
		.

essays: templates/essay.html.tpl $(SRCS_MK)
	$(FOLDR) -t templates/essay.html.tpl \
		-o $(BUILD_DIR) \
		-f essay:true \
		.

comics: templates/comic.html.tpl $(SRCS_MK)
	$(FOLDR) -t templates/comic.html.tpl \
		-o $(BUILD_DIR) \
		-f comic:true \
		.


clean:
	rm -f $(SRCS_MK) $(FONTS_MK)
