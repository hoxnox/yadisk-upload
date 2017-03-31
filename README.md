# Yandex disk file upload utility

Uploads files to yandex disk.

## examples

```shell
./yadisk-upload --auth=AQFFAAADVGl4AAQmatuP25OUtE9DhtFZgBTMDNI ~/file/to/upload /filename_on_disk
```

## yandex oauth token

You should get yandex oatuh token first with write permissions to the
disk. Go the the [link](https://oauth.yandex.ru/client/new). Fill in
"Название", "Описание", select "Яндекс.Диск REST API" and mark all
checkboxes. At the next page you'll have token in the "Пароль" field.

Screenshots:

![screenshot](https://habrastorage.org/files/868/a6d/3fd/868a6d3fde5a4b6bb463d0a296431793.png)
