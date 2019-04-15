!!!!!!!!!!!!!   only autoupdate0.cpp is used for compilation


how to use the version_tag files?

version_tag_old, should be in the same directory, as the autoupdate.exe



*************** version_tag_new *******************************
NewVersion: 11				// declaration of the new version number 
url: www.playspoon.com		


OldVersion: 10				// what's to do if current version is 10
/news.shtml				// files that are downloaded in the tmp directory
{					// start of processing part
mkdir v10
copy tmp\news.shtml v10\test.txt
}					// end of processing part

OldVersion: 9				// next version 
/draid/snap97.jpg
{
mkdir v9
copy tmp\snap97.jpg v9\snap97.jpg
}

end					// just indicates end of file



***************************** processing commands *****************************
the default directory during this command are processed is the calling dir 

	copy sourcefile destinationfile
	
	del file

	mkdir directory

	rmdir directory
	
	system filename 	// execute a file


todo:
* version file downloadable, therefore we have to have it on the server 
* decide if seperate url is in the version_tag_new included or url is a pararmeter of the   final autoupdate function
* check if all files are here 
* put the whole thing in a working routine & create a dumy void main();

maybe todo: 
* crc check of files(all files one crc would be my idea)
* iteration trys if files are missing or corrupted
* replacement for the getnameoffset routine, deals to much with pointers,
  even if I think it works fine
* more useful commands????


070201:
* implement one function
* replace the getnameoffset
* check all commands
* iterations if download fails
 