#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>
#include  <Guid/FileInfo.h>

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
EFI_SYSTEM_TABLE *ST;


/* 追加 */
#define MAX_FILE_NAME_LEN 8
#define MAX_FILE_NUM 10
#define MAX_FILE_BUF 1024

struct FILE {
  unsigned short name[MAX_FILE_NAME_LEN];
};
/* strncpyだが衝突のエラー回避のため */
void strncopy(unsigned short *dst, unsigned short *src, unsigned long long n)
{
  while (n--)
	*dst++ = *src++;
}
/* ここまで */

void hlt(){
  while(1) __asm__("hlt");
}
/* MikanOSのブートローダーより引用 */
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL** root) {
  EFI_STATUS status;
  EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;

  status = gBS->OpenProtocol(
							 image_handle,
							 &gEfiLoadedImageProtocolGuid,
							 (VOID**)&loaded_image,
							 image_handle,
							 NULL,
							 EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (EFI_ERROR(status)) {
    return status;
  }

  status = gBS->OpenProtocol(
							 loaded_image->DeviceHandle,
							 &gEfiSimpleFileSystemProtocolGuid,
							 (VOID**)&fs,
							 image_handle,
							 NULL,
							 EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (EFI_ERROR(status)) {
    return status;
  }

  return fs->OpenVolume(fs, root);
}

EFI_STATUS
EFIAPI
UefiMain (
		  IN EFI_HANDLE ImageHandle,
		  IN EFI_SYSTEM_TABLE *SystemTable
		  )
{
  EFI_STATUS status;	/* 各種EFI_STATUSの返り値を格納する変数 */
  EFI_FILE_PROTOCOL *root;		/* rootを呼び出す */
  unsigned long long buf_size;
  unsigned char file_buf[MAX_FILE_BUF];
  EFI_FILE_INFO *file_info;
  struct FILE file_list[10];
  int idx = 0;


  status = OpenRootDir(ImageHandle, &root);
  if (EFI_ERROR(status)) {
    Print(L"failed to open root directory: %r\n", status);
    hlt();
  }
  Print(L"%r\n", status);

  /* ファイル名を繰り返すことで読み出している */
  while(1) {
	buf_size = MAX_FILE_BUF;
	status = root->Read(root, &buf_size, (void *)file_buf);
	if (EFI_ERROR(status)) {
	  Print(L"Read status:%r\n", status);
	  hlt();
	}
	if (!buf_size) {
	  break;
	}

	file_info = ( EFI_FILE_INFO *)file_buf;
	strncopy(file_list[idx].name, file_info->FileName,
			 MAX_FILE_NAME_LEN - 1);
	file_list[idx].name[MAX_FILE_NAME_LEN - 1] = L'\0';
	Print(file_list[idx].name);
	Print(L" ");
	idx++;
  }


  /* 疑似シェルのlsコマンドとする場合はCloseが必要となるが現状はファイル名を読み出したいだけであるため、実装なし */
  hlt();
  return 0;
}

