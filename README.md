PassWord Generator

Creates secure (I hope) passwords based off of https://xkcd.com/936/. It defaults to only 33 bits of entropy :)

usage:

```
pwg <<< "something"
```

There's a config file. Optional is a key file which is concatinated with the user's input for hashing.

One could set it to something already secret and guarded, like a SSH private key.

My imagined use case it to feed the username and website. Then the private ssh key is hashed together and a password is output.

The innerds

We take input from stdin and run it through SHA-2 256, resulting in a hash

If a key file is provided, we hash it too. We now have two hashes and we concatonate them. We hash this result.

We split the has into of 11-bits numbers (2^11 = 2048 our word list size). We output the corresponding word to this number.

We default to three words for a minimum entropy of 33-bits. The words are five to fourteen characters. Without the wordlist, minimum entropy is 48 bits and maximum is 146 bits. This puts it into dictionary hashcat possible, but web server hacking difficult.
