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

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;

int strcmp(const unsigned short *s1, const unsigned short *s2);

EFI_STATUS
EFIAPI
UefiMain (
		  IN EFI_HANDLE ImageHandle,
		  IN EFI_SYSTEM_TABLE *SystemTable
		  )
{

  EFI_INPUT_KEY Key;
  unsigned short str[3];
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
  while (1)	{
	if(SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)){ /* Print関数のように簡単なものがあればそれにおきかえる */
	  /* CoInで文字を出力をした場合、１を返す */
	  if (Key.UnicodeChar != L'\r') { /* returnキーを押した場合,\rが出力されるため */
		/* returnキーが押されるまでの処理 */
		str[0] = Key.UnicodeChar;
	  }
	  else {
		str[0] = L'\r';
		str[1] = L'\n';
		str[2] = L'\0';
	  }
	  Print(str);
	}
  }
  return 0;
}


