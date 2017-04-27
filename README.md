# Yandex disk file upload utility

Uploads files to yandex disk. Doesn't use curl. Extremely fast and low
memory consuming. Authentication by token - doesn't need user/password
with full access to the profile.

## examples

```shell
./yadisk-upload --auth=AQFFAAADVGl4AAQmatuP25OUtE9DhtFZgBTMDNI ~/file/to/upload /filename_on_disk
```

## building

The project uses cmake build system. You can set `WITH_SYSTEM_*` to 0 to
build some dependencies with the project. Example on Linux:

```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_SYSTEM_BOOST=0 -DWITH_SYSTEM_OPENSSL=0 -DWITH_SYSTEM_ZLIB=0 ..
make
```

### standalone

You can build on standalone a server. Make the following directory
structure with the dependencies (see following) and define environment
variable `VENDOR_DIR`.

```
vendoring
├── boost.org
│   └── boost
│       └── boost_1_64_0.tar.gz
├── docopt
│   └── docopt.cpp
│       ├── docopt.cpp-0.6.2.tar.gz
├── easylogging
│   └── easyloggingpp
│       └── easyloggingpp_v9.84.tar.gz
├── libressl.org
│   └── libressl
│       └── libressl-2.5.3.tar.gz
├── openssl
    └── openssl
        └── openssl-1.1.0c.tar.gz
```
### Microsoft Visual Studio

OpenSSL configure script need perl installed. To simplify build on
with Visual Studio LibreSSL is used. It's better to build dependencies
with the project:

```sh
mkdir build
cd build
cmake -DWITH_SYSTEM_LIBRESSL=0 -DWITH_SYSTEM_BOOST=0
```

Launch Visual Studio on `yadisk-upload.sln` chose build type `Release`
and build `ALL_BUILD` subproject.

Tested on Visual Studio 2017.

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

