/**
 * EOT URL Setter.
 * Author: azki. (azki@azki.org)
 * Spec: http://www.w3.org/Submission/EOT/
 * Base code by eot-utils 1.0-1 (http://eot-utils.sourcearchive.com/documentation/1.0-1/main.html)
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FSTYPE_RESTRICTED 0x0002
#define FSTYPE_PREVIEW 0x0004
#define FSTYPE_EDITABLE 0x0008
#define FSTYPE_NOSUBSETTING 0x0100
#define FSTYPE_BITMAP 0x0200

typedef struct {
	unsigned long EOTSize;
	unsigned long FontDataSize;
	unsigned long Version;
	unsigned long Flags;
	unsigned char FontPANOSE[10];
	unsigned char Charset;
	unsigned char Italic;
	unsigned long Weight;
	unsigned short fsType;
	unsigned short MagicNumber; /* = 0x504C */
	unsigned long UnicodeRange1;
	unsigned long UnicodeRange2;
	unsigned long UnicodeRange3;
	unsigned long UnicodeRange4;
	unsigned long CodePageRange1;
	unsigned long CodePageRange2;
	unsigned long CheckSumAdjustment;
	unsigned long Reserved1;/* = 0 */
	unsigned long Reserved2;/* = 0 */
	unsigned long Reserved3;/* = 0 */
	unsigned long Reserved4;/* = 0 */
	unsigned short Padding1;/* = 0 */
	unsigned short FamilyNameSize;
	unsigned char *FamilyName;/* in UTF-16! */
	unsigned short Padding2;/* = 0 */
	unsigned short StyleNameSize;
	unsigned char *StyleName; /* in UTF-16! */
	unsigned short Padding3;/* = 0 */
	unsigned short VersionNameSize;
	unsigned char *VersionName; /* in UTF-16! */
	unsigned short Padding4;/* = 0 */
	unsigned short FullNameSize;
	unsigned char *FullName;/* in UTF-16! */
	unsigned short Padding5;/* = 0 */
	unsigned short RootStringSize;
	unsigned char *RootString;/* in UTF-16! */
	unsigned long RootStringCheckSum;
	unsigned long EUDCCodePage;
	unsigned short Padding6;/* = 0 */
	unsigned short SignatureSize;
	unsigned char *Signature;
	unsigned long EUDCFlags;
	unsigned long EUDCFontSize;
	unsigned char *EUDCFontData;
} EOT_header;


/* write_4le -- write a little endian unsigned long */
static bool write_4le(FILE *f, unsigned long x)
{
	unsigned char s[4];

	s[0] = x & 0x000000ff;
	s[1] = (x & 0x0000ff00) >> 8;
	s[2] = (x & 0x00ff0000) >> 16;
	s[3] = x >> 24;
	return fwrite(s, 1, 4, f) == 4;
}

/* write_2le -- write a little endian unsigned short */
static bool write_2le(FILE *f, unsigned short x)
{
	unsigned char s[2];

	s[0] = x & 0x00ff;
	s[1] = x >> 8;
	return fwrite(s, 1, 2, f) == 2;
}

/* write_1 -- write one byte to file f */
static bool write_1(FILE *f, unsigned char x)
{
	return fwrite(&x, 1, 1, f) == 1;
}


/* initialize_EOT_header -- fill an EOT struct with consistent values */
static void initialize_EOT_header(EOT_header *h)
{
	h->EOTSize = 120;
	h->FontDataSize = 0;
	h->Version = 0x00020002;
	h->Flags = 0;
	memset(h->FontPANOSE, 0, 10);
	h->Charset = 0;
	h->Italic = 0;
	h->Weight = 400;/* "normal" */
	h->fsType = 0x0;/* "installable" */
	h->MagicNumber = 0x504C;
	h->UnicodeRange1 = 0;
	h->UnicodeRange2 = 0;
	h->UnicodeRange3 = 0;
	h->UnicodeRange4 = 0;
	h->CodePageRange1 = 0;
	h->CodePageRange2 = 0;
	h->CheckSumAdjustment = 0;
	h->Reserved1 = 0;
	h->Reserved2 = 0;
	h->Reserved3 = 0;
	h->Reserved4 = 0;
	h->Padding1 = 0;
	h->FamilyNameSize = 0;
	h->FamilyName = NULL;
	h->Padding2 = 0;
	h->StyleNameSize = 0;
	h->StyleName = NULL;
	h->Padding3 = 0;
	h->VersionNameSize = 0;
	h->VersionName = NULL;
	h->Padding4 = 0;
	h->FullNameSize = 0;
	h->FullName = NULL;
	h->Padding5 = 0;
	h->RootStringSize = 0;
	h->RootString = NULL;
	h->RootStringCheckSum = 0;
	h->EUDCCodePage = 0;
	h->Padding6 = 0;
	h->SignatureSize = 0;
	h->Signature = NULL;
	h->EUDCFlags = 0;
	h->EUDCFontSize = 0;
	h->EUDCFontData = NULL;
}
/* putUTF8 -- write a character to stdout in UTF8 encoding */
static void putUTF8(long c)
{
	if (c <= 0x7F) {												/* Leave ASCII encoded */
		putchar(c);
	} else if (c <= 0x07FF) {							 /* 110xxxxx 10xxxxxx */
		putchar(0xC0 | (c >> 6));
		putchar(0x80 | (c & 0x3F));
	} else if (c <= 0xFFFF) {							 /* 1110xxxx + 2 */
		putchar(0xE0 | (c >> 12));
		putchar(0x80 | ((c >> 6) & 0x3F));
		putchar(0x80 | (c & 0x3F));
	} else if (c <= 0x1FFFFF) {						 /* 11110xxx + 3 */
		putchar(0xF0 | (c >> 18));
		putchar(0x80 | ((c >> 12) & 0x3F));
		putchar(0x80 | ((c >> 6) & 0x3F));
		putchar(0x80 | (c & 0x3F));
	} else if (c <= 0x3FFFFFF) {									/* 111110xx + 4 */
		putchar(0xF8 | (c >> 24));
		putchar(0x80 | ((c >> 18) & 0x3F));
		putchar(0x80 | ((c >> 12) & 0x3F));
		putchar(0x80 | ((c >> 6) & 0x3F));
		putchar(0x80 | (c & 0x3F));
	} else if (c <= 0x7FFFFFFF) {								 /* 1111110x + 5 */
		putchar(0xFC | (c >> 30));
		putchar(0x80 | ((c >> 24) & 0x3F));
		putchar(0x80 | ((c >> 18) & 0x3F));
		putchar(0x80 | ((c >> 12) & 0x3F));
		putchar(0x80 | ((c >> 6) & 0x3F));
		putchar(0x80 | (c & 0x3F));
	} else {													/* Not a valid character... */
		printf("<%ld>", c);
	} 
}

/* write_EOT_header -- write an EOT header to file f */
static bool write_EOT_header(FILE *f, EOT_header h)
{
	if (!write_4le(f, h.EOTSize) ||
		!write_4le(f, h.FontDataSize) ||
		!write_4le(f, h.Version) ||
		!write_4le(f, h.Flags) ||
		fwrite(h.FontPANOSE, 1, 10, f) != 10 ||
		!write_1(f, h.Charset) ||
		!write_1(f, h.Italic) ||
		!write_4le(f, h.Weight) ||
		!write_2le(f, h.fsType) ||
		!write_2le(f, h.MagicNumber) ||
		!write_4le(f, h.UnicodeRange1) ||
		!write_4le(f, h.UnicodeRange2) ||
		!write_4le(f, h.UnicodeRange3) ||
		!write_4le(f, h.UnicodeRange4) ||
		!write_4le(f, h.CodePageRange1) ||
		!write_4le(f, h.CodePageRange2) ||
		!write_4le(f, h.CheckSumAdjustment) ||
		!write_4le(f, h.Reserved1) ||
		!write_4le(f, h.Reserved2) ||
		!write_4le(f, h.Reserved3) ||
		!write_4le(f, h.Reserved4) ||
		!write_2le(f, h.Padding1) ||
		!write_2le(f, h.FamilyNameSize) ||
		fwrite(h.FamilyName, 1, h.FamilyNameSize, f) != h.FamilyNameSize ||
		!write_2le(f, h.Padding2) ||
		!write_2le(f, h.StyleNameSize) ||
		fwrite(h.StyleName, 1, h.StyleNameSize, f) != h.StyleNameSize ||
		!write_2le(f, h.Padding3) ||
		!write_2le(f, h.VersionNameSize) ||
		fwrite(h.VersionName, 1, h.VersionNameSize, f) != h.VersionNameSize ||
		!write_2le(f, h.Padding4) ||
		!write_2le(f, h.FullNameSize) ||
		fwrite(h.FullName, 1, h.FullNameSize, f) != h.FullNameSize)
		return false;

	switch (h.Version) {
	case 0x0001000:
		break;
	case 0x00020001:
		if (!write_2le(f, h.Padding5) ||
			!write_2le(f, h.RootStringSize) ||
			fwrite(h.RootString, 1, h.RootStringSize, f) != h.RootStringSize)
			return false;
		break;
	case 0x00020002:
		if (!write_2le(f, h.Padding5) ||
			!write_2le(f, h.RootStringSize) ||
			fwrite(h.RootString, 1, h.RootStringSize, f) != h.RootStringSize ||
			!write_4le(f, h.RootStringCheckSum) ||
			!write_4le(f, h.EUDCCodePage) ||
			!write_2le(f, h.Padding6) ||
			!write_2le(f, h.SignatureSize) ||
			fwrite(h.Signature, 1, h.SignatureSize, f) != h.SignatureSize ||
			!write_4le(f, h.EUDCFlags) ||
			!write_4le(f, h.EUDCFontSize) ||
			fwrite(h.EUDCFontData, 1, h.EUDCFontSize, f) != h.EUDCFontSize)
			return false;
		break;
	default:
		return false;
	}
	return true;
}

/* read_ulong -- read a little endian unsigned long */
static bool read_ulong(FILE *f, unsigned long *x)
{
	unsigned char s[4];

	if (fread(s, 1, 4, f) != 4) return false;
	*x = (s[3] << 24) + (s[2] << 16) + (s[1] << 8) + s[0];
	return true;
}


/* read_ushort -- read a little endian unsigned short */
static bool read_ushort(FILE *f, unsigned short *x)
{
	unsigned char s[2];

	if (fread(s, 1, 2, f) != 2) return false;
	*x = (s[1] << 8) + s[0];
	return true;
}

/* read_EOT_header -- read the header part of an EOT file */
static bool read_EOT_header(FILE *f, EOT_header *h)
{
	if (!read_ulong(f, &(h->EOTSize))) return false;
	if (!read_ulong(f, &(h->FontDataSize))) return false;
	if (!read_ulong(f, &(h->Version))) return false;
	if (!read_ulong(f, &(h->Flags))) return false;
	if (fread(h->FontPANOSE, 1, 10, f) != 10) return false;
	if (fread(&(h->Charset), 1, 1, f) != 1) return false;
	if (fread(&(h->Italic), 1, 1, f) != 1) return false;
	if (!read_ulong(f, &(h->Weight))) return false;
	if (!read_ushort(f, &(h->fsType))) return false;
	if (!read_ushort(f, &(h->MagicNumber))) return false;
	if (h->MagicNumber != 0x504C) return false;
	if (!read_ulong(f, &(h->UnicodeRange1))) return false;
	if (!read_ulong(f, &(h->UnicodeRange2))) return false;
	if (!read_ulong(f, &(h->UnicodeRange3))) return false;
	if (!read_ulong(f, &(h->UnicodeRange4))) return false;
	if (!read_ulong(f, &(h->CodePageRange1))) return false;
	if (!read_ulong(f, &(h->CodePageRange2))) return false;
	if (!read_ulong(f, &(h->CheckSumAdjustment))) return false;
	if (!read_ulong(f, &(h->Reserved1))) return false;
	if (h->Reserved1 != 0) return false;
	if (!read_ulong(f, &(h->Reserved2))) return false;
	if (h->Reserved2 != 0) return false;
	if (!read_ulong(f, &(h->Reserved3))) return false;
	if (h->Reserved3 != 0) return false;
	if (!read_ulong(f, &(h->Reserved4))) return false;
	if (h->Reserved4 != 0) return false;
	if (!read_ushort(f, &(h->Padding1))) return false;
	if (h->Padding1 != 0) return false;
	if (!read_ushort(f, &(h->FamilyNameSize))) return false;
	if (!(h->FamilyName = (unsigned char *) malloc(h->FamilyNameSize))) return false;
	if (fread(h->FamilyName, 1, h->FamilyNameSize, f) != h->FamilyNameSize) return false;
	if (!read_ushort(f, &(h->Padding2))) return false;
	if (h->Padding2 != 0) return false;
	if (!read_ushort(f, &(h->StyleNameSize))) return false;
	if (!(h->StyleName = (unsigned char *) malloc(h->StyleNameSize))) return false;
	if (fread(h->StyleName, 1, h->StyleNameSize, f) != h->StyleNameSize) return false;
	if (!read_ushort(f, &(h->Padding3))) return false;
	if (h->Padding3 != 0) return false;
	if (!read_ushort(f, &(h->VersionNameSize))) return false;
	if (!(h->VersionName = (unsigned char *) malloc(h->VersionNameSize))) return false;
	if (fread(h->VersionName, 1, h->VersionNameSize, f) != h->VersionNameSize) return false;
	if (!read_ushort(f, &(h->Padding4))) return false;
	if (h->Padding4 != 0) return false;
	if (!read_ushort(f, &(h->FullNameSize))) return false;
	if (!(h->FullName = (unsigned char *) malloc(h->FullNameSize))) return false;
	if (fread(h->FullName, 1, h->FullNameSize, f) != h->FullNameSize) return false;

	switch (h->Version) {
	case 0x0001000:
		break;
	case 0x00020001:
		if (!read_ushort(f, &(h->Padding5))) return false;
		if (h->Padding5 != 0) return false;
		if (!read_ushort(f, &(h->RootStringSize))) return false;
		if (!(h->RootString = (unsigned char *) malloc(h->RootStringSize))) return false;
		if (fread(h->RootString, 1, h->RootStringSize, f) != h->RootStringSize) return false;
		break;
	case 0x00020002:
		if (!read_ushort(f, &(h->Padding5))) return false;
		if (h->Padding5 != 0) return false;
		if (!read_ushort(f, &(h->RootStringSize))) return false;
		if (!(h->RootString = (unsigned char *) malloc(h->RootStringSize))) return false;
		if (fread(h->RootString, 1, h->RootStringSize, f) != h->RootStringSize) return false;
		// break;
		if (!read_ulong(f, &(h->RootStringCheckSum))) return false;
		if (!read_ulong(f, &(h->EUDCCodePage))) return false;
		if (!read_ushort(f, &(h->Padding6))) return false;
		if (h->Padding6 != 0) return false;
		if (!read_ushort(f, &(h->SignatureSize))) return false;
		if (!(h->Signature = (unsigned char *) malloc(h->SignatureSize))) return false;
		if (fread(h->Signature, 1, h->SignatureSize, f) != h->SignatureSize) return false;
		if (!read_ulong(f, &(h->EUDCFlags))) return false;
		if (!read_ulong(f, &(h->EUDCFontSize))) return false;
		if (!(h->EUDCFontData = (unsigned char *) malloc(h->EUDCFontSize))) return false;
		if (fread(h->EUDCFontData, 1, h->EUDCFontSize, f) != h->EUDCFontSize) return false;
		break;
	default:
		return false;
	}
	return true;
}

#define CS_XORKEY 0x50475342
unsigned long GetByteCheckSum( unsigned char* pucBuffer,
 unsigned short cbLength )
{
int i;
unsigned long ulCS = 0;
for(i = 0; i < cbLength; i++) {
ulCS += pucBuffer[i];
}
return ulCS ^ CS_XORKEY;
}


/* add_rootstring -- add a URL to the EOT header */
static void add_rootstring(char *url, EOT_header *h)
{
	unsigned long j, len;
	len = 2 * strlen(url) + 2;
	if (!(h->RootString = (unsigned char *)realloc(h->RootString, h->RootStringSize + len)))
		return;

	j = h->RootStringSize;
	while (*url) {/* Simplistic conversion to UTF-16LE */
		h->RootString[j++] = *url;
		h->RootString[j++] = '\0';
		url++;
	}
	h->RootString[j++] = '\0';
	h->RootString[j++] = '\0';
	h->RootStringSize += len;
	h->RootStringCheckSum = GetByteCheckSum(h->RootString, h->RootStringSize);
	h->EOTSize += len;
}

static void resetRootString(EOT_header *h)
{
	h->EOTSize -= h->RootStringSize;
	h->RootStringSize = 0;
	h->RootStringCheckSum = GetByteCheckSum(h->RootString, h->RootStringSize);
}


/* usage -- print usage message and exit */
static void usage(char *progname)
{
	fprintf(stderr, "usage: %s input_file [output_file [URL [URL...]]]\n\n", progname);
	fprintf(stderr, "ex1. read urls.\n\t%s in.eot\n", progname);
	fprintf(stderr, "ex2. set urls.\n\t%s in.eot out.eot http://daum.net http://uie.daum.net\n\n", progname);
	exit(1);
}
static void errx(char * a, void * b){
	printf(a, b);
	exit(-1);
}
static void printRootString(EOT_header h){
	unsigned short i;
	long c;
	for (i = 0; i < h.RootStringSize; i += 2) /* UTF-16LE -> UTF-8 */
		if ((c = h.RootString[i] + 256 * h.RootString[i+1])) putUTF8(c);
		else putUTF8(' ');
}

int main(int argc, char *argv[])
{
	FILE *f1;
	FILE *f2;
	EOT_header header;
	int c;
	unsigned long i; 

	if (argc < 2) usage(argv[0]);
	if (!(f1 = fopen((const char *)argv[1], "rb"))) return -1;
	initialize_EOT_header(&header);
	//READ
	if (!read_EOT_header(f1, &header)) errx("Could not read font file %s.", argv[1]);
	if (argc == 2) {
		printRootString(header);
		exit(1);
	}

	//SET
	resetRootString(&header);
	for (i = 3; i < argc; i++) add_rootstring(argv[i], &header);

	if (!(f2 = fopen((const char *)argv[2], "wb"))) return -1;
	//WRITE
	if (!(header.fsType & FSTYPE_EDITABLE) &&
		!(header.fsType & FSTYPE_PREVIEW) &&
		header.fsType & FSTYPE_RESTRICTED
	) errx("%s does not allow embedding.", argv[1]);
	if (header.fsType & FSTYPE_BITMAP) errx("Unsupported (%s requires bitmap embedding).", argv[1]);
	
	if (!write_EOT_header(f2, header)) errx("Could not write EOT file", argv[2]);
	for (i = 0; i < header.FontDataSize; i++) {
		c = getc(f1);
		if (c == EOF) errx("Font data size != file read?", argv[2]);
		putc(c, f2);
	}
	fclose(f1);
	fclose(f2);
	printf("complete.");
	return 0;
}
