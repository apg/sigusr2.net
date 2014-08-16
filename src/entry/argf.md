% Code Blogging: ARGF in Python
% python, code-blogging
% 2010-06-23


    """ARGF from Ruby in Python.

    Released into the public domain by Andrew Gwozdziewycz, 2010

    """


    import sys, os



    class _ARGF(object):


        def __init__(self):

            self.lineno = 0

            self.file = None


        def __iter__(self):

            return self.next()


        def next(self):

            files = filter(os.path.isfile, sys.argv[1:])

            pairs = [(f, open(f)) for f in files] \

                if files else [('STDIN', sys.stdin)]


            for name, fobj in pairs:

                self.file = name


                for line in fobj.xreadlines():

                    self.file = 'STDIN'

                    self.lineno += 1

                    yield line


    ARGF = _ARGF()


**Update:** It was pointed out that I didn't handle `lineno` correctly; I
fixed it.

