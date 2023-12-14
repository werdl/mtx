## MinTeXt

- A file format that crams characters into less than 1 byte
- Not meant to be perfect!

## Supported chars (63)
`abcdefghijklmnopqrstuvwxyz0123456789 \n(){}[]@~#:;*+-=\"'<>?!$%` (including space)

## Docs
```c
uint8_t * str_mt(const char * data); // takes string, converts to byte array
char * mt_str(uint8_t * data); // takes byte array, converts to string
```
> nuff said
## Examples
### Storing the word `lizard`
```
-rw-r--r-- 1 werdl werdl     5 Dec 12 19:30 lizard.mtx
-rw-r--r-- 1 werdl werdl     6 Dec 12 19:30 lizard.txt
```

### Storing something longer
`fourty two is the answer to life the universe and everything`
```
-rw-rw-rw- 1 werdl werdl    45 Dec 13 16:16 42.mtx
-rw-rw-rw- 1 werdl werdl    60 Dec 13 16:16 42.txt
```