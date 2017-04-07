# Yandex disk file upload utility

Uploads files to yandex disk. Doesn't use curl. Extremely fast and low
memory consuming. Authentication by token - doesn't need user/password
with full access to the profile.

## examples

```shell
./yadisk-upload --auth=AQFFAAADVGl4AAQmatuP25OUtE9DhtFZgBTMDNI ~/file/to/upload /filename_on_disk
```

## yandex oauth token

You should get yandex oatuh token first with write permissions to the
disk. Go the the [link](https://oauth.yandex.ru/client/new). Fill in
"Название", "Описание", select "Яндекс.Диск REST API", mark all
checkboxes and click "Подставить URL для разработки".

![register app](https://habrastorage.org/files/8f4/b13/752/8f4b13752ebd49d9a01b81a43bb84b56.png)

At the next page you'll get `client_id`.

![get client id](https://habrastorage.org/files/02b/5b1/929/02b5b1929ba548c0b1fe6fc29699d13f.png)

Go to the link 

	https://oauth.yandex.ru/authorize?response_type=token&client_id=<paste client id here>

![get client id](https://habrastorage.org/files/444/740/b57/444740b571a44c81bbfee3f48e8098aa.png)

Click "Разрешить", and you'll get auth token

![get client id](https://habrastorage.org/files/57d/4d7/166/57d4d7166341413a912f67dc15157967.png)

