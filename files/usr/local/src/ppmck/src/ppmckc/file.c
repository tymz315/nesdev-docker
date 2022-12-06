/************************************************************/
/*															*/
/************************************************************/
#include	<stddef.h>
#include	<ctype.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

extern int message_flag;

char*asterisk_macro[256];

/* -------------------------------------------------------------
 * �������p�X�f���~�^���ǂ������f
 * �����t�@�C�������Ή�?
 * -------------------------------------------------------------*/
int is_pathdelimiter(char c)
{
#if defined(UNIX) || defined(LINUX)
	return (c == '/');
#else
	return (c == '\\' || c == '/');
#endif
}



/*--------------------------------------------------------------
	������ptr���p�X/�t�@�C���l�[��/�g���q�ɕ���(path,name,ext�ɃR�s�[)
 Input:

 Output:
	none
--------------------------------------------------------------*/
void splitPath( const char *ptr, char *path, char *name, char *ext )
{
	const char *temp, *eopath, *eoname;

	if( *ptr == '\0' ) {								/* �G���[�`�F�b�N */
		*path = '\0';
		*name = '\0';
		*ext = '\0';
		return;
	}

	temp = ptr;
	while( *temp != '\0' ) ++temp;						/* ��x�Ō�܂Ői�� */
	eoname = temp;
	while( --temp != ptr ) {
		if( is_pathdelimiter(*temp) ) {
			temp++;
			break;
		}
	}
	eopath = temp;
	while( *temp != '\0' ) {
		if ( *temp == '.' ) {
			eoname = temp;
		}
		++temp;
	}
	strncpy( path, ptr, eopath-ptr );
	*(path+(eopath-ptr)) = '\0';
	strncpy( name, eopath, eoname-eopath );
	*(name+(eoname-eopath)) = '\0';
	strcpy( ext, eoname );
}



/*--------------------------------------------------------------
  	path + name + ext �� ptr �ɃR�s�[
 Input:

 Output:
	none
--------------------------------------------------------------*/
void makePath( char *ptr, const char *path, const char *name, const char *ext )
{
	strcpy( ptr, path );
	strcat( ptr, name );
	strcat( ptr, ext );
}



/*--------------------------------------------------------------
	�t�@�C���T�C�Y�����߂�
 Input:
	char	fname	:FILE NAME
 Return:
	�t�@�C���T�C�Y(0�̏ꍇ�̓G���[)
--------------------------------------------------------------*/
int getFileSize( char *ptr )
{
	FILE	*fp;
	int		size;

	fp = fopen( ptr, "rb" );
	if( fp == NULL ) return 0;

	fseek( fp, 0L, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0L, SEEK_SET );
	fclose( fp );

	return size;
}

/*------------------------------------------------------------*/
void	asterisk_macro_expand(char ic,char**p) {
	char*pp=asterisk_macro[ic];
	for(;*pp && *pp!='\n';pp++) {
		if(*pp=='*') {
			asterisk_macro_expand(*++pp,p);
		} else {
			*(*p)++=*pp;
		}
	}
}


/*--------------------------------------------------------------
	���s�R�[�h��ύX���Ȃ���t�@�C����ǂݍ���
 Input:
	char	fname	:FILE NAME
 Return:
	�|�C���^(NULL�̏ꍇ�̓G���[)
--------------------------------------------------------------*/
char *readTextFile( char *filename )
{
	
	FILE *fp;
	int size, sizeb, line_idx;
	char *top, *p;
	int c, c2;
	char line_fc=0;
	/* �t�@�C���I�[�v�� */
	fp = fopen(filename, "rb");
	if (!fp) {
		if (message_flag == 0) {
			printf( "%s : �t�@�C�����J���܂���\n", filename );
		} else {
			printf( "%s : Can't open file\n", filename );
		}
		return NULL;
	}
	/* �T�C�Y���擾 */
	fseek(fp, 0L, SEEK_END);
	sizeb = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	/*
	if (sizeb == 0) {
		fclose(fp);
		if (message_flag == 0) {
			printf( "%s : �t�@�C���T�C�Y��0�ł�\n", filename );
		} else {
			printf( "%s : File is empty\n", filename );
		}
		return NULL;
	}
	*/
	/* �o�b�t�@���쐬 */
readTextFile_again:
	top = malloc((sizeb + 1) * sizeof(c)); /* �����Ƀi��������t������̂�1�o�C�g���� */
	if (top == NULL) {
		fclose(fp);
		if (message_flag == 0) {
			printf( "%s : ���������m�ۏo���܂���ł���\n", filename );
		} else {
			printf( "%s : Out of memory\n", filename );
		}
		return NULL;
	}

	/*
	size = fread(str, 1, sizeb, fp);
	*/
	line_idx = 1;
	p = top;
	while (1) {
		if(p>top+sizeb) {
			free(top);
			fseek(fp, 0L, SEEK_SET);
			sizeb<<=1;
			goto readTextFile_again;
		}
		c = fgetc(fp);
		if (c == EOF) {
			/* file end */
			*p++ = '\0';
			break;
		} else if (c == '\x0d') {
			line_fc=0;
			/* CRLF or CR ? */
			c2 = fgetc(fp);
			if (c2 == '\x0a'){
				/* CRLF */;
				*p++ = '\n';
				line_idx++;
			} else if (c2 == EOF){
				/* CR */;
				/* file end */
				*p++ = '\n';
				*p++ = '\0';
				line_idx++;
				break;
			} else {
				/* CR */;
				*p++ = '\n';
				line_idx++;
				c2 = ungetc(c2, fp);
				if (c2 == EOF) {
					/* ungetc fail */
					fclose(fp);
					free(top);
					printf("%s : ungetc failed\n", filename);
					return NULL;
					
				}
			}
		} else if (c == '\x0a'){
			line_fc=0;
			/* LF */
			*p++ = '\n';
			line_idx++;
		} else if (c == '\0') {
			/* may be binary file */
			fclose(fp);
			free(top);
			if (message_flag == 0) {
				printf("%s : �s�K�؂ȕ���'\\0'��������܂���(�����炭�o�C�i���t�@�C�����J����)\n", filename);
			} else {
				printf("%s : Illegal charcter '\\0' found (file may be a binary file)\n", filename);
			}
			return NULL;
		} else if(c=='*' && !line_fc) {
			*p++ = ';';
			c2 = fgetc(fp);
			asterisk_macro[c2]=p;
			line_fc='#';
		} else if(c=='*' && line_fc!='#' && line_fc!='?') {
			asterisk_macro_expand(fgetc(fp),&p);
		} else {
			/* other char */
			*p++ = c;
			if(line_fc!=' ' && line_fc!='\t' && !line_fc) line_fc=c;
		}
	}
	fclose(fp);
	size = (p - top) / sizeof(c);
	/*
	printf("read %d byte -> store %d byte (\\0 ���܂�) \n", sizeb, size);
	printf("read %d line\n", line_idx);
	*/
	if (size > sizeb + 1) {
		free(top);
		if (message_flag == 0) {
			printf("%s : �m�ۂ����������ȏ�Ƀt�@�C����ǂݍ��݂܂���\n", filename);
		} else {
			printf("%s : File was read exceeding allocated memory\n", filename);
		}
		return NULL;
	}
	return top;
}





/*--------------------------------------------------------------

--------------------------------------------------------------*/
static int dmcpath_inited = 0;
static char dmcpath[10][512];

static void initDmcPath(void)
{
	char *p,*pl;
	int i, l;
	dmcpath_inited = 1;
	p = getenv("DMC_INCLUDE");
	if (p == NULL)
		return;
	
	for (i = 0; i < 10; i++) {
		pl = strchr(p, ';');

		if (pl == NULL)
			l = strlen(p);
		else
			l = pl-p;

		if (l == 0) {
			dmcpath[i][0] = '\0';
		} else {
			strncpy(dmcpath[i],p,l);
			p += l;
			while (*p == ';') p++;
		}

		if (!is_pathdelimiter(dmcpath[i][strlen(dmcpath[i])])) {
			strcat(dmcpath[i], "/");
		}
	}
}



FILE *openDmc(char *name)
{
	FILE 	*fileptr;
	char	testname[512];
	int	i;

	if (!dmcpath_inited) {
		initDmcPath();
	}
	
	fileptr = fopen(name, "rb");
	if (fileptr != NULL) return(fileptr);

	for (i = 0; i < 10; i++) {
		if (strlen(dmcpath[i])) {
			strcpy(testname, dmcpath[i]);
			strcat(testname, name);
	
			fileptr = fopen(testname, "rb");
			if (fileptr != NULL) break;
		}
	}

	return (fileptr);
}