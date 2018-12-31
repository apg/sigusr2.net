% Simple Django View For Dynamic Text Replacement
% 
% 2006-06-30

I just discovered [this][1] and though I like the idea, I don't use PHP.
However, the same thing could be done with Django, and the Python Imaging
Library.

Please note that, this is tested, but in no way optimal, and will remain that
way until I get time to do it right, or someone else does it right, or already
has done it right....

First things first:

Define some aliases for fonts in settings.py


    DYNAMIC_FONT_ALIASES = {

       'a': '/path/to/some/truetypefont.ttf',

    }


Then, we define a url in urls.py


    # other views here

    (r'^(?P<fontalias>.*)/$', 'testtest.many.views.view_header'),


Then, we create the view... in this case view_header. This is the part that
needs fixing.


    from django.http import HttpResponse

    from testtest import settings

    import Image, ImageFont, ImageDraw

    def view_header(request, fontalias):

        try:

            fontfile = settings.DYNAMIC_FONT_ALIASES[fontalias]

        except:

            return HttpResponse("not supported")


        if request.GET.has_key('text'):

            header = request.GET['text']

        else:

            header = 'Hello world'


        imf = ImageFont.truetype(fontfile, 20)

        size = imf.getsize(header)

        im = Image.new("RGB", size)

        draw = ImageDraw.Draw(im)

        draw.text((0, 0), header, font=imf)


        # anyone know a better way than saving first? I guess we get built in
cache this way....

        im.save("/tmp/aaaa", "PNG")

        a = open("/tmp/aaaa", "rb")

        return HttpResponse(content=a.read(), mimetype="image/png")


And there you have it. Start up your application and point yourself to
[here][2], and marvel at the glory that is, Django dynamic headers... or what
could be used as dynamic headers with some css.

   [1]: http://artypapers.com/csshelppile/pcdtr/

   [2]:
http://localhost:8000/a/?text=This%20is%20a%20dynamic%20django%20header

