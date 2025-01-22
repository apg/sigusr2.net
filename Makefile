URL = http://sigusr2.net
NAME = SIGUSR2
SSG = ./bin/ssg4
RSSG = ./bin/rssg
RSYNC_TARGET = apg@peter.sigusr2.net:/var/www/htdocs/sigusr2.net

all: mkdirs posts rss images

clean:
	rm -rf ./build

mkdirs:
	@mkdir -p ./build

posts:
	$(SSG) ./src ./build $(NAME) $(URL)

rss: src/feed.html
	$(RSSG) ./src/feed.html > ./build/feed.xml
	cp ./build/feed.xml ./build/rss.xml

images:
	cp -a ./i/ ./build/
	cp favicon.png ./build/

devel:
	cd build/ && python3 -m http.server

sync:
	cd build/ && rsync -vrRz --rsh=/usr/bin/ssh . $(RSYNC_TARGET)
