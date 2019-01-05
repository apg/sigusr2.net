# Notes on OpenBSD

## DNS Resolution with rebound(8)

```
$ cat /etc/dhcpclient.conf
prepend domain-name-servers 127.0.0.1;
```

