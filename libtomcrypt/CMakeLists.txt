# CFLAGS += -c -I./ -Wall -Wsign-compare -W -Wno-unused -Wshadow 

add_library(libtomcrypt STATIC 
	mpi.c
	keyring.c
	gf.c
	mem.c
	sprng.c
	ecc.c
	base64.c
	dh.c
	rsa.c
	bits.c
	yarrow.c
	cfb.c
	ofb.c
	ecb.c
	ctr.c
	cbc.c
	hash.c
	tiger.c
	sha1.c
	md5.c
	md4.c
	md2.c
	sha256.c
	sha512.c
	xtea.c
	aes.c
	des.c
	safer_tab.c
	safer.c
	safer+.c
	rc4.c
	rc2.c
	rc6.c
	rc5.c
	cast5.c
	noekeon.c
	blowfish.c
	crypt.c
	prime.c
	twofish.c
	packet.c
	hmac.c
	strings.c
	rmd128.c
	rmd160.c
	skipjack.c
	omac.c
	dsa.c)

target_include_directories(libtomcrypt PUBLIC ".")

