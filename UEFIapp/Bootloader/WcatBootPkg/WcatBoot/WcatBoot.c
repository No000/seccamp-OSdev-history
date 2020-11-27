/* memo */
/* ファイルの中身を読み出す場合、簡易的なテスト目的であればBufferでいいかもしれないが */
/* 本格的に読み出す場合、メモリを確保する必要がある(gBS->AllocatePool) */

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
#include  <stdint.h>
/* ELFヘッダーは */

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
EFI_SYSTEM_TABLE *ST;


/* 追加 */
/* ここはfile_infoを利用して動的に取得できるようにする */
#define MAX_FILE_NAME_LEN 8
#define MAX_FILE_NUM 10
#define MAX_FILE_BUF 1024		/* test用のファイルバッファの大きさ */

struct FILE {
  uint16_t name[MAX_FILE_NAME_LEN];
}__attribute__((packed));
/* strncpyだが名前衝突のエラーがうっとおしいため */
/* 参考にした場合のミスが目立つ場合戻すこと */
void strncopy(unsigned short *dst, unsigned short *src, unsigned long long n)
{
  while (n--)
	*dst++ = *src++;
}


/* while(1)で止めるのは気持ち悪いので */
void hlt(){
  while(1) __asm__("hlt");
}

/* MikanOSのブートローダーより引用 */
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root) {
  EFI_STATUS status;
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
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
  UINTN buf_size;
  uint8_t file_buf[MAX_FILE_BUF];
  EFI_FILE_INFO *file_info;
  struct FILE file_list[10];
  int index = 0;
  EFI_INPUT_KEY key;
  UINTN waitIndex;


  /* 画面クリアを行う */
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);


  /* ロゴの表示（エスケープシーケンスに注意） */
  Print(L" __       __                        __       ______    ______  \n");
  Print(L"/  |  _  /  |                      /  |     /      \\  /      \\ \n");
  Print(L"$$ | / \\ $$ |  _______   ______   _$$ |_   /$$$$$$  |/$$$$$$  |\n");
  Print(L"$$ |/$  \\$$ | /       | /      \\ / $$   |  $$ |  $$ |$$ \\__$$/ \n");
  Print(L"$$ /$$$  $$ |/$$$$$$$/  $$$$$$  |$$$$$$/   $$ |  $$ |$$      \\ \n");
  Print(L"$$ $$/$$ $$ |$$ |       /    $$ |  $$ | __ $$ |  $$ | $$$$$$  |\n");
  Print(L"$$$$/  $$$$ |$$ \\_____ /$$$$$$$ |  $$ |/  |$$ \\__$$ |/  \\__$$ |\n");
  Print(L"$$$/    $$$ |$$       |$$    $$ |  $$  $$/ $$    $$/ $$    $$/ \n");
  Print(L"$$/      $$/  $$$$$$$/  $$$$$$$/    $$$$/   $$$$$$/   $$$$$$/  \n");
  Print(L"                                                               \n");
  Print(L"                                                               \n");
  Print(L"                                                               \n");

  /* カーネルブートするかのチェックを行う */
  Print(L"Kernel boot(press RET)\n");
  SystemTable->BootServices->WaitForEvent(1,&(SystemTable->ConIn->WaitForKey),&waitIndex);//入力があるまで待機



  while (1) {
	SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn,&key);
    if(key.UnicodeChar != '\r'){
	  Print(L"KernelBoot Start\n");
	  break;
	}
  }

  
  status = OpenRootDir(ImageHandle, &root);
  if (EFI_ERROR(status)) {
    Print(L"failed to open root directory: %r\n", status);
    hlt();
  }
  Print(L"OpenRootDir:%r\n", status);

  /* ファイル名を繰り返すことで読み出している */
  Print(L"RootDirectory: ");
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
	strncopy(file_list[index].name, file_info->FileName,
			 MAX_FILE_NAME_LEN - 1);
	file_list[index].name[MAX_FILE_NAME_LEN - 1] = L'\0';
	Print(file_list[index].name);
	Print(L" ");
	index++;
  }
  Print(L"\n");


  /* 疑似シェルのlsコマンドとする場合等はCloseが必要となるが現状はファイル名を読み出したいだけであるためなし */

  /* Elfファイルの情報の抜き出しを行う */
  /* 流れは */
  /* １，kernelを解析するためにバイナリデータをメモリ上に展開するバッファを確保する */
  /* ２，バッファ内にkernelをいれる */
  /* ４，定義したElfheaderの構造体に紐づける */
  /* ５，構造体経由で読み込みを行う */
  /* ELFヘッダーの構造体は別のヘッダーファイルで定義する */



  /* kernelのファイルを開く */
  EFI_FILE_PROTOCOL *kernel_file;

  Print(L"kernelfile is a.out\n");
  status = root->Open(
					  root, &kernel_file, L"\\a.out", /* a.out(linuxバイナリ)をkernelに見立ててデータを読み出す */
					  EFI_FILE_MODE_READ, 0);

  Print(L"kernelfile open:%r\n", status);
  if (EFI_ERROR(status)) {
	hlt();
  }


  
  VOID *kernel_buffer;			/* kernelのバイナリ読み出し用 */
  UINTN kernel_file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
  UINT8 kernel_file_info_buf[kernel_file_info_size];
  status = kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &kernel_file_info_size,kernel_file_info_buf);
  Print(L"Getinfo: %r\n", status);
  if (EFI_ERROR(status)) {
	hlt();
  }

  EFI_FILE_INFO* kerne_file_info = (EFI_FILE_INFO*)kernel_file_info_buf;
  UINTN kernel_file_size = kerne_file_info->FileSize;


  status = gBS->AllocatePool(EfiLoaderData, kernel_file_size, (void **)&kernel_buffer);
  Print(L"AllocatePool: %r\n", status);
  if (EFI_ERROR(status)) {
	hlt();
  }
  
  status = kernel_file->Read(kernel_file, &kernel_file_size, kernel_buffer);
  Print(L"kernelRead: %r\n", status);
  if (EFI_ERROR(status)) {
	hlt();
  }

  /* status = ReadFile(kernel_file, &kernel_buffer); */
  uint8_t * kernele_buf_test = (uint8_t *)kernel_buffer;
  int i;
  for (i = 0; i < 16; i++){
	Print(L"%08x\n", kernele_buf_test[i]);
  }



	
  hlt();
  return 0;
}
