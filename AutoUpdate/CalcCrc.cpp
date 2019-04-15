#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <winsock.h>
#include <fstream.h>
#include <iostream.h>
#include <direct.h>


#define VERSIONTAGNEW "version_tag_new.vtg"
#define RAWVERSIONTAG "raw_version_tag.txt"

long calccrc(LPCSTR file);
LPCSTR removepath(LPCSTR lpFileName);		

	
int main( )
{
char argumentbuffer1[256],key;
long crc[20];
int updatev;
 

 ifstream RAW_file(RAWVERSIONTAG, ios::in); 							// open new version file
 

	RAW_file >> argumentbuffer1;  

	do {
													
		RAW_file >> argumentbuffer1;									// read version keyword 
		if (strcmpi(argumentbuffer1,"OldVersion:") == 0) {
			RAW_file >> updatev; 
			cout << "\nOldVersion: " << updatev;
			crc[updatev]=0;	
			RAW_file >> argumentbuffer1;    
			while(strcmpi(argumentbuffer1,"{") != 0) {				
  				crc[updatev]+=calccrc(removepath(argumentbuffer1));
				RAW_file >> argumentbuffer1;									// read next file to delete
			}
			cout << "       CRCSUM: " << crc[updatev];

        }
	} while ((strcmpi(argumentbuffer1,"end")) != 0);

 cout << "\n";	
 RAW_file.close();	


 ifstream RA_file(RAWVERSIONTAG, ios::in); 						
 ofstream NEW_file(VERSIONTAGNEW, ios::out);

 	RA_file >> argumentbuffer1;									// read  
	NEW_file << argumentbuffer1 << " ";							// write

 do {											
		if (strcmpi(argumentbuffer1,"OldVersion:") == 0) {
			RA_file >> updatev; 
			NEW_file << updatev << " ";
			NEW_file << crc[updatev] << " ";
		}
			RA_file >> argumentbuffer1;   
			NEW_file << argumentbuffer1 << " ";
	} while ((strcmpi(argumentbuffer1,"end")) != 0);



 RA_file.close();	
 NEW_file.close();
 cin >> key;
 return 0;

}



long calccrc(LPCSTR file)					// calc the checksum of a file 
{
  char buffer;
  long crcsum=0;
 

  ifstream source(file, ios::in|ios::nocreate|ios::binary);
  if(!source)
  {
    cerr << "\nKonnte die Datei "<< file << "  nicht öffnen";
    return 0;
  }


  while(source.get(buffer))
	  crcsum+=buffer;
     
  source.close();
  return crcsum;
}


LPCSTR removepath(LPCSTR lpFileName)			// remove the '/' in the filename if possible
{
LPCSTR lpShortFileName;

lpShortFileName = strrchr(lpFileName,'/');

if (lpShortFileName == 0)
	lpShortFileName = lpFileName;
else
	lpShortFileName++;

return lpShortFileName;

}
