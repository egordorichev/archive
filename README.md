# archive

A simple library for creating and extracting archives with lit.

## Building

```bash
sudo apt install libarchive-dev
cmake .
make
sudo make install
```

## Usage

```js
if (!Archive.create("test.txt", "archive.zip")) {
    print("Failed to create the archive")
}

if (!Archive.extract("archive.zip", "archive/")) {
    print("Failed to open the archive")
}
```
