URL = http://sigusr2.net
NAME = SIGUSR2
SSG = ./bin/ssg4
RSSG = ./bin/rssg
RSYNC_TARGET = apg@sigusr2.net:/var/www/htdocs/sigusr2.net

all: mkdirs posts rss images

clean:
	rm -rf ./build

mkdirs:
	@mkdir -p ./build

posts:
	$(SSG) src build $(NAME) $(URL)

rss: src/index.html
	$(RSSG) ./src/index.html > ./build/rss.xml

images:
	cp -R ./i ./build/i/
	cp favicon.png ./build/

devel:
	cd build/ && python3 -m http.server

sync:
	cd build/ && rsync -vrRz --rsh=/usr/bin/ssh . $(RSYNC_TARGET)
