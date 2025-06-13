//--------------------------------------------------------------------------------------------------------------------------------------------//
//        Project Title: Customized Virtual File System (CVFS)
//        Description  : This project is a customized virtual file system that
//                       simulates file creation, reading, writing, lseek, close 
//                       operations and management within a controlled memory space.
//        Author       : Amol R. Mane
//        Date         : 30 January 2025
//--------------------------------------------------------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                [ CUSTOMIZED VIRTUAL FILE SYSTEM ]
//--------------------------------------------------------------------------------------------------------------------------------------------//

//  CVFS - Customized Virutal File System it is a virtual memeory based project based on operating system file system.
/* Basically Operating System does five tasks such as - 1] File Management  
                                                        2] Memory Management    
                                                        3] Process Management
                                                        4] CPU Scheduling 
                                                        5] Hardware Abstraction

   As i wanted to know the operating systems internals i have made a project which is based on (File Management).
   Operating system manages all the files and stores them on the hard disk. 
   But this project CVFS does not create files on the hard disk as it is a virutal memory based project it manages all the files on the (Random Access Memory). 
   The file which is created and managed by OS is permenant which is on the hard disk .
   But CVFS file is temprory as it is created on the RAM after the file gets terminated we cannot get back that file as it is created virtually. 
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                     [ CVFS.cpp ]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    The program is written in (C) programming language. 
    But as you see the extension of file name is (.cpp)
    The extension of file is .cpp but the program inside file is completely wirtten in C language.
    Because we are able to use a function two times with the same name which is called (name mangling) and it is not allowed in C language.
    But in C++ we can use a same function name with different parameters which is called (Function Overloading).
    GetFDFromName is used two times in this program with paramerter (name) and (command[1]) for that purpose we used function overloading concept in cpp.
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


#include<stdio.h>   // used for input output operations such as printf,scanf

#include<stdlib.h>  // used for memroy allocation such as malloc,free

#include<string.h>  // used to perform string operations such as strcpy,strncpy,strlen to check or copy file names,etc.

#include<unistd.h>  // POSIX (Portable Operating System Interface) header file used mostly in Unix/Linux-based systems. 
                    // It provides access to the operating system API, particularly system calls such as read,write,lssek,close,fork,sleep,etc.

#include<iostream>  // used to perform operation needed by .cpp program

//#include<io.h>    // only used on windows operating system to perform operations like open,read,write,creat,close,lssek,unlink.


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                          [MACROS]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*  
    #define are the user defined macros which are used to create or define shortcuts which can be used directly in program anywhere with that name  
    so that we do not have to mention the values again and again it provides better redability of the code and maintains easyness.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


#define MAXINODE 50         // It represents that how much maximum number of files that i can create i.e 50 INODES... Each inode represents a file.

#define READ 1              // It represents that READ 1 mode gives permission to read the file only.
#define WRITE 2             // It represents that Write 2 mode gives permission to write in the file only.

#define MAXFILESIZE 2048    // It represents the maximum number of bytes can be stored or used in a file that is 2048 bytes.(2 KB)

#define REGULAR 1           // A REGULAR file is such a file that the user creates and writes content into.
#define SPECIAL 2           // SPECIAL file is such a file which is used for specail device files or files with restricted operations scush asDirectries,system drivers.

#define START 0             // It is like lssek function START means that the beginning of the file data.
#define CURRENT 1           // CURRENT means that current position of that data.
#define END 2               // END means that the end point of that file data.


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                       [BOOTBLOCK]
//--------------------------------------------------------------------------------------------------------------------------------------------//
//
//   As this is customized virutal file system so there is not need to create our bootblock as it is already present in our computer.
//   On which we will perform the file operations as bootblock is already running after that we are performing program on RAM.
//
//--------------------------------------------------------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                      [SUPERBLOCK]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    The superblock is a data structure of inodes that stores global information about the entire file system.
    Information such as - 1] TotalInodes
                          2] FreeInodes     
                          3] Used Inodes
                          4] Total Blocks
                          5] Total Free Blocks                  
    It acts like a manager for virtual file system.
    The operating system is fetcning the data from the superblock for file manipulation.
    Superblock points to DILB which contains linked list of inodes which contains all the inofrmation of file such as file name, block no., file size, permissions, inode no. 

                                +------------------+
                                |   SUPERBLOCK     |
                                |------------------|
                                | TotalInodes: 50  |
                                | FreeInode : 47   |
                                +------------------+
                                    |
                                    |
                                    |
                                    V
                                +--------+      +--------+      +--------+
                                | INODE 1| ---> | INODE 2| ---> | INODE 3| ---> ... up to 50
                                |--------|      |--------|      |--------|
                                | FileType = 1  | FileType = 0  | FileType = 1
                                | FileName = A  | (free slot)   | FileName = B
                                +--------+      +--------+      +--------+

*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


typedef struct superblock
{
    int TotalInodes;        // Total number of inodes (50)
    int FreeInode;          // Tracks available file slots (dynamic)
}SUPERBLOCK, *PSUPERBLOCK;


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                  [INODE]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Inode is a structure created by operating system which holds inofrmation of file such as -

                                            +------------------------------+
                                            | INODE                        |
                                            |------------------------------|
                                            | FileName       = "test.txt"  |
                                            | InodeNumber    = 1           |
                                            | FileSize       = 2048        |
                                            | FileActualSize = 120         |
                                            | FileType       = 1 (REGULAR) |
                                            | Buffer         = [data]      |
                                            | LinkCount      = 1           |
                                            | RefrenceCount  = 1           |
                                            | Permission     = 3           |
                                            | next ----------> [next INODE]|
                                            +------------------------------+

    When we create a new file then new inode gets allocated and the that we write in file gets written inside the data block.  
    Incore Inode Table (IIT) points to Data Block (DB) which contains the actual data of that file.  
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


typedef struct inode
{
    char FileName[50];      // Name of the file (up to 49 characters + null)
    int InodeNumber;        // Unique ID for the inode (e.g., 1 to 50)
    int FileSize;           // Max size allowed for the file (2048 Bytes)
    int FileActualSize;     // Actual number of bytes written in the file
    int FileType;           // 1 = REGULAR, 2 = SPECIAL
    char *Buffer;           // Pointer to memory holding file data which is called Data Block.
    int LinkCount;          // How many links point to this file
    int RefrenceCount;      // How many times this file is opened
    int Permission;         // 1 = Read, 2 = Write, 3 = Read + Write
    struct inode *next;     // Pointer to the next inode (linked list)
}INODE, *PINODE, **PPINODE;


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                        [FILETABLE]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    FileTable is a table which holds information about the file.  
    File table contains four things such as - 1] Mode 
                                              2] Offset
                                              3] count
                                              4] pointer to inode (incore inode table - IIT)

                    +-----------------------------+            +----------------------+
                    |       FILETABLE             |            |        INODE         |
                    |-----------------------------|            |----------------------|
                    | readoffset    = 0           |            | FileName = "test.txt"|
                    | writeoffset   = 5           |            | FileActualSize = 5   |
                    | count         = 1           | ---------->| Buffer = [Hello]     |
                    | mode          = 2 (WRITE)   |            | InodeNumber = 1      |
                    | ptrinode ----> INODE addr   |            +----------------------+
                    +-----------------------------+
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;              // count represents the number of active opens for that file.(Active References of a file)
    int mode;               // 1 = READ, 2 = WRITE, 3 = READ+WRITE
    PINODE ptrinode;        // pointer pointing to the file's inode
}FILETABLE, *PFILETABLE;


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                            [UFDT]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Every process gets allocated UAREA- User Area.  
    Inside UAREA one array is created named as UFDT - User File Descriptor Table.  
    Inside UFDT address of FileTable entry gets stored.  
    such as -   
                ____0______1______2_______3_____4_______5_____6______7____  
                |  -    |  -   |  -   | 100  | 200  | 300  | 400  | 500  |
                |_______|______|______|______|______|______|______|______|.........  
                 STDIN   STDOUT STDERR

                 fig - UAREA - user area inside this UFDT array is present which contains the addresses which points to filetable.  
                 Here first three block in Operating System are reserved for standared input,output,error purpose.  
                 Actual index storing addresses starts from index 3.  


                       UFDTArr[3] ──┬───────────────────────────────┐
                                    ▼                               │
                                FILETABLE                           │
                                ┌─────────────┐                     │
                                │ mode = 3    │                     │
                                │ readoffset  │                     │
                                │ writeoffset │                     │
                                │ ptrinode ─────────────────────────┘
                                └─────────────┘
                                    │
                                    ▼
                                INODE
                                ┌──────────────┐
                                │ FileName     │
                                │ FileSize     │
                                │ Buffer ─────► [file content here] - (DataBlock)
                                │ Permission   │
                                └──────────────┘

        Internally these all points to each other -  UFDTArr -> FILETABLE -> INODE(IIT) -> DATABLOCK
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


typedef struct ufdt
{
    PFILETABLE ptrfiletable;    // Points to FILETABLE of an open file
}UFDT;

UFDT UFDTArr[50];               // Supports up to 50 simultaneously open files. Holds 50 entries — each corresponds to a file descriptor. 
                                // Each entry is either NULL (file not open) or points to a FILETABLE.
SUPERBLOCK SUPERBLOCKobj;       // Global superblock object
PINODE head = NULL;             // Head of inode linked list


//--------------------------------------------------------------------------------------------------------------------------------------------//
//    Chain of dependency 
//    UFDTArr[fd] → FILETABLE → INODE → File Buffer(DataBlock)
//--------------------------------------------------------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                        [man()]
//--------------------------------------------------------------------------------------------------------------------------------------------//

/*  Function Name : man
    Description   : Displays the manual for help information to a given command
                    in the Customized Virtual File System (CVFS).
    Parameters    : 1] name : The name of the command for which help information is requested.
    Return Value  : None(void)
    Logic         : Step 1: Check if the command name is NULL. If yes, return immediately.
                    Step 2: Used (strcmp) to compare the command name with known commands.
                    Step 3: If the command matches, print its description and usage.
                    Step 4: If the command does not match any known commands, no manual entry available.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//

void man(char *name)
{
    if(name == NULL)    return;

    if(strcmp(name,"create") == 0)
    {
        printf("Description : Used to create new regular file\n");
        printf("Usage       : create File_name Permission\n");
    }
    else if(strcmp(name,"read") == 0)
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage       : read File_name No_Of_Bytes_To_Read\n");
    }
    else if(strcmp(name,"write") == 0)
    {
        printf("Description : Used to write into regular file\n");
        printf("Usage       : write File_name\n After this enter the data that we want to write\n");
    }
    else if(strcmp(name,"ls") == 0)
    {
        printf("Description : Used to list all files information of files\n");
        printf("Usage       : ls\n");
    }
    else if(strcmp(name,"stat") == 0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage       : stat File_name\n");
    }
    else if(strcmp(name,"fstat") == 0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage       : fstat File_Descriptor\n");
    }
    else if(strcmp(name,"truncate") == 0)
    {
        printf("Description : Used to remove data from file\n");
        printf("Usage       : truncate File_name\n");
    }
    else if(strcmp(name,"open") == 0)
    {
        printf("Description : Used to open existing file\n");
        printf("Usage       : open File_name mode\n");
    }
    else if(strcmp(name,"close") == 0)
    {
        printf("Description : Used to close opened file\n");
        printf("Usage       : close File_name\n");
    }
    else if(strcmp(name,"closeall") == 0)
    {
        printf("Description : Used to close all opened files\n");
        printf("Usage       : closeall\n");
    }
    else if(strcmp(name,"lseek") == 0)
    {
        printf("Description : Used to change file offset\n");
        printf("Usage       : lseek File_Name ChangeInOffset StartPoint\n");
    }
    else if(strcmp(name,"rm") == 0)
    {
        printf("Description : Used to delete the file\n");
        printf("Usage       : rm File_Name\n");
    }
    else 
    {
        printf("ERROR : No manual entry available.\n");
    }
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                           [DisplayHelp()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : DisplayHelp
    Description   : Displays a list of all available commands in the (CVFS)
                    with the description of command.  
    Parameters    : None
    Return Value  : None
    Logic         : Prints the list of commands with their descriptions using printf.
                    Each command is displayed with its usage.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//

void DisplayHelp()
{
    printf("ls       : To List out all files\n");
    printf("clear    : To clear console\n");
    printf("open     : To open the file\n");
    printf("close    : To close the file\n");
    printf("closeall : To close all opened files\n");
    printf("read     : To Read the contents from file\n");
    printf("write    : To Write contents into file\n");
    printf("exit     : To Terminate file system\n");
    printf("stat     : To Display information of file using name\n");
    printf("fstat    : To Display information of file using file descriptor\n");
    printf("truncate : To Remove all data from file\n");
    printf("rm       : To Delete the file\n");
}

//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                          [GetFDFromName]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : GetFDFromName
    Description   : Searches the User File Descriptor Table (UFDT) to find the file descriptor (FD)
                    index with the specified file name.
    Parameters    : 1] name : The name of the file to search for in the UFDT.

    Return Value  : If the file is found, it returns the file descriptor index (0 to 49).
                    If the file is not found, it returns -1.
    
    Logic         : Step 1- Loop through the UFDTArr array of size 50.
                    Step 2- Check if the ptrfiletable is not NULL meaning a file is opened.
                    Step 3- Compare the given filename with the FileName in the inode.
                    Step 4- If a match is found, return the index as the file descriptor.
                    Step 5- If the loop completes without finding a match, return -1.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//

int GetFDFromName(char *name)
{
    int i = 0;

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name) == 0)
                break;
        i++;
    }

    if(i == 50)     return -1;          // File not found
    else            return i;           // Returns file descriptors index
}

//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                              [GetInode]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : Get_Inode
    Description   : Searches for a file with the specified name in Disk Inode List Block (DILB),
                    and returns the pointer to its inode structure if file gets found.
    Parameters    : 1] name : The name of the file to search for in the linked list of inodes.

    Return Value  : If the file is found, it returns a pointer to the corresponding inode.
                    If the file is not found then it returns NULL.
    Logic         :
                    Step 1: Start from the head of the inode list.
                    Step 2: Loop through each inode node and compare the name with the inode's FileName.
                    Step 3: If a match is found, the loop breaks, and the pointer to that inode is returned.
                    Step 4: If the end of the list is reached without finding a match then it returns NULL.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//

PINODE Get_Inode(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
        return NULL;
    
    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName) == 0)
            break;
        temp = temp->next;
    }
    return temp;          // Returns the pointer pointing to inode if file name matches to the file inside inode if file name does not match temp returns NULL
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                          [DataInodeListBlock]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : CreateDILB
    Description   : This function initializes the Disk Inode List Block (DILB) dynamically 
                    creating and linking INODE structures in a linked list format.
                    The list consists fixed number of inodes defined by MAXINODE.
    Parameters    : None
    Return Value  : None
    Logic         :
                    Step 1: A loop runs from 1 to MAXINODE.
                    Step 2: For each iteration it dynamically allocates memory for INODE.
                    Step 3: Initializes the inode with - 
                            LinkCount, ReferenceCount, FileType, FileSize, Buffer, and InodeNumber.
                    Step 4: Links the new inode to the list -
                            If it is the first node it is assigned as head.
                            If not it gets to the end of the list.
                    Step 5: Continues this process until the list contains MAXINODE inodes.
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//

void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;             // newn is a pointer used to point to inode where inode contains inofrmation of file
    PINODE temp = head;             // temp is a pointer which currently initialized with  head

    while(i <= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));       // memory allocation for newn

        newn->LinkCount = 0;
        newn->RefrenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;

        newn->Buffer = NULL;
        newn->next = NULL;

        newn->InodeNumber = i;

        if(temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    printf("DILB created successfully\n");
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                          [InitialiseSuperBlock]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : InitialiseSuperBlock
    Description   : Initializes the SuperBlock structure and User File Descriptor Table (UFDT)
                    of the Customized Virtual File System (CVFS).
                    It sets all file table pointers in the UFDT to NULL and marks all inodes as free.
    Parameters    : None
    Return Value  : None
    Logic         :
                    Step 1: Loop through the UFDTArr array which represents all possible open files.
                    Step 2: Set each entry's ptrfiletable pointer to NULL indicating no file is open.
                    Step 3: Initialize the SuperBlock with TotalInodes and FreeInodes.
                            TotalInodes: Set to MAXINODE representing the total number of inodes available.
                            FreeInode  : Set to MAXINODE indicating all inodes are initially free.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//

void InitialiseSuperBlock()
{
    int i = 0;
    while(i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                   [CreateFile]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : CreateFile
    Description   : Creates a new file with the specified name and permission.
                    And inode and a file table entry for managing file access.
    Parameters    : 1]name       : The name of the file to be created.
                    2]permission : The access permissions (Read, Write, or Read+Write).
    Return Value  : 
                    returns i as File Descriptor (index in UFDT array) if successful.
                    return -1 if parameters are invalid (e.g., NULL name or wrong permission).
                    return -2 if there are no free inodes available.
                    return -3 if a file with the same name already exists.
                    return -4 if memory allocation for file table or buffer fails.
    
    Logic         : Step 1: Validate the parameters.
                    Step 2: Check if there are free inodes in the SuperBlock.
                    Step 3: Ensure the file does not already exist.
                    Step 4: Traverse the inode linked list to find a free inode.
                    Step 5: Find a free index in the UFDT array.
                    Step 6: Allocate memory for a file table.
                    Step 7: Initialize the file table structure with offsets and permissions.
                    Step 8: Initialize inode metadata.
                    Step 9: Allocate memory for the file's buffer.
                    Step 10: Decrement the free inode count in the SuperBlock.
                    Step 11: Return the file descriptor index to indicate success.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


int CreateFile(char *name, int permission)
{
    int i = 0;
    PINODE temp = head;

    if(name == NULL || permission == 0 || permission > 3)
        return -1;                          // Invalid parameters

    if(SUPERBLOCKobj.FreeInode == 0)
        return -2;                          // No inode available 

    if(Get_Inode(name) != NULL)             // This function is used to check only that if the file that we want to create is already present in inode or not? If file is not present in inode with the name we entered then we can create file otherwise if the file is present in the inode with that name then we cannot able to create the file.
    return -3;                              // File already exists
        
    // Finds a free inode
    while(temp != NULL)
    {
        if(temp->FileType == 0)
        break;
        temp = temp->next;   
    }
        
    // Find a free file descriptor
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        break;
        i++;
    }
    
    // Allocate memory for FILETABLE
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)                                    //  CODE Modified 
    return -4;  // Memory allocation failure                                //  CODE Modifided by me Amol which indicates memory failure
    
    // Initializing filetable
    UFDTArr[i].ptrfiletable->count = 1;             // count is how many times the file is opened.
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    
    UFDTArr[i].ptrfiletable->ptrinode = temp;
    
    // Initializing inode   
    // I can also use syntax such as (temp->FileType = REGULAR,temp->ReferenceCount = 1) like this but above syntax give more redability and easy to understand deeply.
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->RefrenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->Permission = permission;
    
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);
    if(UFDTArr[i].ptrfiletable->ptrinode->Buffer == NULL)
    return -4;                                                                      // Memory allocation failure
    
    (SUPERBLOCKobj.FreeInode)--;                                                    // Reduce free inode count

    return i;                                                                       // Returns file descriptor index.
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                  [rm_file()] 
//              Deletes the file by giving the name and from that name it gets FD and unlinks or deletes the file.
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : rm_file
    Description   : Removes deletes a file from the Customized Virtual File System (CVFS).
                    It releases the allocated memory and marks the inode as free.
    Parameters    : 1] name : The name of the file to be removed.
    
    Return Value  :  0 if the file is successfully removed.
                     -1 if the file is not found.
    
    Logic         : Step 1: Get the File Descriptor (FD) of the file using function GetFDFromName().
                    Step 2: If the FD is not found return -1.
                    Step 3: Decrease the LinkCount of the inode associated with the file.
                    Step 4: If LinkCount reaches 0 then - 
                                                        Mark the inode's FileType as 0 (free).
                                                        Deallocate the memory for the file table.
                    Step 5: Increase the free inode count in the SuperBlock.
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


// rm_File(Demo.txt)
int rm_file(char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);
    if(fd == -1)
        return -1;                              // Invalid file not found

    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
        // free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
        // UFDTArr[fd].ptrfiletable->ptrinode->Buffer = NULL;
        free(UFDTArr[fd].ptrfiletable);
    }

    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++;
}

//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                       [ReadFile()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
//
//     This function should be modified because there is bug which is not seen directly 
//      the bug is in return value isize it is returning the actual value given by user but not returning how much actual bytes were read
//      update in (if else block with return read_size in if block and return isize in else block ) because we know that if block read only the 
//      the data available to read from offset but in else block it reads all the data requested by user with bytes i.e iszie.
//
/*
    Function Name : ReadFile
    Description   : Reads the specified number of bytes from a file in the Customized Virtual File System (CVFS)
                    into the provided buffer, starting from the current read offset.
     
    Parameters    : 1] fd    : File Descriptor of the file to read.
                    2] arr   : Buffer to store the read contents.
                    3] isize : Number of bytes to read from the file.
     
    Return Value :  If successful it returns the number of bytes read that is isize.
                    return -1 if the file does not exist.
                    return -2 if there is a permission issue (file not opened in read mode).
                    return -3 if the read offset has reached the end of the file.
                    return -4 if the file is not a regular file.
    
    Logic        :  Step 1: Check if the File Descriptor (fd) is valid and the file is open.
                    Step 2: Verify that the file is opened in READ or READ+WRITE mode.
                    Step 3: Ensure the inode has read permissions.
                    Step 4: Check if the read offset has reached the file's actual size.
                    Step 5: Validate the file is of type REGULAR.
                    Step 6: Calculate the readable size based on the current offset.
                    Step 7: Perform the read:
                                - If readable size is less than isize then only available bytes are read.
                                - If readable size is more than or equal to isize then exactly isize bytes are read.
                    Step 8: Return the number of bytes successfully read.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


int ReadFile(int fd, char *arr, int isize)
{
    int read_size = 0;

    if(UFDTArr[fd].ptrfiletable == NULL)    return -1;                          // Represents File not existing 

    if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)      return -2;      // Represents permission denied.

    if(UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ+WRITE)      return -2;

    // if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)    return -3;    UPDATED TO BELOW STATEMENT WITH > THAN SIGN
    if(UFDTArr[fd].ptrfiletable->readoffset >= UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)    return -3;      // Represents that the file is reached at its end.

    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)     return -4;

    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    if(read_size < isize)
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),read_size);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
        // return read_size; 
    }
    else
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),isize);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + isize;
        // return isize;
    }

    return isize;   // number of bytes successfully read.
}   


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                       [WriteFile()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : WriteFile
    Description   : Writes the specified number of bytes from a provided buffer (arr) 
                    to the file represented by the file descriptor (fd). The data is 
                    written starting from the current write offset.
     
    Parameters    : 1] fd    : File Descriptor of the file to write data.
                    2] arr   : The data buffer containing the data to be written.
                    3] isize : Number of bytes to write into the file.
     
    Return Value  : If successful it returns the number of bytes written (isize).
                    return -1 if permission is denited of WRITE.
                    return -2 if the write offset has reached the maximum file size.
                    return -3 if the file is not a regular file.
    
    Logic         : Step 1: Validate if the file is opened with write permissions.
                    Step 2: Validate if the inode allows writing.
                    Step 3: Check if the write offset is at the maximum size.
                    Step 4: Ensure the file is a regular file.
                    Step 5: strncpy() to copy data from the buffer to the file's memory space.
                    Step 6: Update the write offset and the file's actual size.
                    Step 7: Return the number of bytes written as confirmation.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


int WriteFile(int fd, char *arr, int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != (READ+WRITE)))     return -1;                                // Permission denied

    if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != (READ+WRITE)))   return -1;  // Permission denied

    if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)      return -2;                  // No space to write

    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)   return -3;                  // Not a regular file

    // if((UFDTArr[fd].ptrfiletable->writeoffset + isize) > MAXFILESIZE)    return -4;           // shuold be updated changed by Amol Mane // new one  // It may cause buffer overflow.

    // if(fd < 0 || arr == NULL) return -5;         // should be updated by me amol mane. // new one

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;
    
    return isize;       // returns the number of bytes written
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                            [CloseAllFile()] 
//              closes one file but should close all file due to break keyword in future i can modify it
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
  Function Name : CloseAllFile
  Description   : Closes all open files present in the User File Descriptor Table (UFDT)
                  by resetting their read and write offsets and decrementing their reference counts.
  Parameters    : None
  Return Value  : None
  Logic         :
                  Step 1: Loop through the UFDT array which contains up to 50 possible file entries.
                  Step 2: For each valid entry (not NULL) we perform -
                                Reset readoffset and writeoffset to 0.
                                Decrement the ReferenceCount of inode.
                  Step 3: The loop has a break which causes it to stop after the **first open file** is found.

 Edge Cases     : (This is important cases in logic building of the program).
                    - If the file is not open (i.e., ptrfiletable is NULL) it safely skips.
                    - If only one file is open then it works fine. But if multiple files are open then the current logic fails.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


void CloseAllFile()
{
    int i = 0;
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->RefrenceCount)--;
            break;                                                  // break keyword terminates the while loop and exit it.
                                                                    // Only one file gets deleted due to break keyword
                                                                    // If we remove break it travels to all FD and close all open files.
        }
        i++;
    }
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                    [LseekFile()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : LseekFile
    Description   : Changes the current read/write offset of an open file in the Customized Virtual File System (CVFS).
                    It allows repositioning of the file pointer within the file.
     
    Parameters    : 1] fd   : File Descriptor of the file for reposition that is lseek.
                    2] size : The number of bytes to move the offset.
                    3] from : The reference position for offset movement. 
                    It can be:
                                START   (0): Move from the beginning of the file.
                                CURRENT (1): Move relative to the current offset.
                                END     (2): Move relative to the end of the file.
     
    Return Value  : return 0 if successful.
                    return -1 if:
                                 The file descriptor is invalid.
                                 The "from" value is incorrect.
                                 The resulting offset is out of bounds.
    
    Logic         : Step 1: Validate the file descriptor and "from" parameter.
                    Step 2: Check if the file is opened in READ or READ + WRITE mode.
                    Step 3: Based on the value of from:
                                                        If CURRENT: Set the offset to the current position of the file.
                                                        If START  : Set the offset to the beginning of the file.
                                                        If END    : Set the offset to the end of the file.
  Edge Cases    :
                    If the file is not open it returns -1.
                    If the calculated offset goes beyond the file size it returns -1.
                    If the offset is negative (less than 0) it returns -1.
*/
//--------------------------------------------------------------------------------------------------------------------------------------------//


int LseekFile(int fd, int size, int from)
{
    if((fd < 0) || (from > 2))      return -1;                      // unable to perfrom lseek
    if(UFDTArr[fd].ptrfiletable == NULL)    return -1;              // unable to perfrom lseek

    if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) > ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)))   return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)    return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))   return -1;
            if(size < 0)   return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if(from == END)
        {
            if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size) > MAXFILESIZE)    return -1;
            if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size) < 0)              return -1; 
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)      return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)                return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
        }
        else if(from == START)
        {
            if(size > MAXFILESIZE)      return -1;
            if(size < 0)                return -1;
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = size;
        }
        else if(from == END)
        {
            if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size) > MAXFILESIZE)     return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)                            return -1;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    return 0;       // If return zero is not wirtten there may be direct termination when we use END LSEEK as there is no return -1 which will indicate error.
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                     [OpenFile()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : OpenFile
    Description   : Opens an existing file in the Customized Virtual File System (CVFS) 
                    with the specified access mode (Read, Write, or Read+Write).
                    It sets up the necessary file table and inode pointers.
     
    Parameters    : name : The name of the file to open.
                    mode : The mode in which the file should be opened.
                            READ (1),WRITE (2),READ + WRITE (3)
                                          
    Return Value  : File Descriptor index in UFDT array if successful.
                    return -1 if parameters are invalid NULL name or mode is 0.
                    return -2 if the file is not present in the system.
                    return -3 if the requested mode is not allowed due to permission restrictions.
                    return -4 if memory allocation for the file table fails.

    Logic         : Step 1: Validate the parameters (file name and mode).
                    Step 2: Search the inode list for the specified file name.
                    Step 3: Verify that the file permissions match the requested mode.
                    Step 4: Find a free slot in the User File Descriptor Table (UFDT).
                    Step 5: Allocate memory for the file table entry.
                    Step 6: Initialize file table properties like mode and offsets.
                    Step 7: Set the offsets based on the opening mode.
                    Step 8: Link the inode to the file table and increment the reference count.
                    Step 9: Return the index of (File Descriptor) if successful.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


int OpenFile(char *name, int mode)
{
    int i = 0;
    PINODE temp = head;

    if(name == NULL || mode <= 0)
        return -1;                  // Incorrect parameters
    
    temp = Get_Inode(name);
    if(temp == NULL)
        return -2;                  // File not present

    if(temp->Permission < mode)
        return -3;                  // Permission denied

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL)     return -4;                  // Memory allocation failure
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;

    if(mode == READ+WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    UFDTArr[i].ptrfiletable->ptrinode = temp;
    (UFDTArr[i].ptrfiletable->ptrinode->RefrenceCount)++;

    return i;                                                           // returns file descriptors index.
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                     [CloseFileByName()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : CloseFileByName
    Description   : Closes an open file in the Customized Virtual File System (CVFS) 
                    based on the file name provided. It releases the memory 
                    allocated for the file table and decrements the inode's reference count.
     
    Parameters    : name : The name of the file to be closed.
     
    Return Value  : return 0 if the file is successfully closed.
                    returnm-1 if the file is not found in the User File Descriptor Table (UFDT).
    Logic         :
                    Step 1: Get File Descriptor (FD) using the file name.
                    Step 2: If the file is not found then return -1.
                    Step 3: Reset the readoffset and writeoffset to 0.
                    Step 4: Decrement the Reference Count of the inode.
                    Step 5: Free the memory allocated to the file table.
                    Step 6: Mark the UFDT entry as NULL indicating that it is closed.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//
  

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if(i == -1)
        return -1;                                          // Invalid parameter

    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->RefrenceCount)--;

    // free(UFDTArr[i].ptrfiletable);
    // UFDTArr[i].ptrfiletable = NULL;

    return 0;
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                       [ls_file()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : ls_file
    Description   : Lists all the files currently present in the Customized Virtual File System (CVFS).
                    It displays the file name, inode number, file size, and link count.
    Parameters    : None
    Return Value  : None
    Logic         : Step 1: Check if there are any files in the system by comparing 
                            the number of free inodes with the maximum number of inodes.
                            If all are free, it means no files are created yet.
                    Step 2: Print the header information for better readability.
                    Step 3: Traverse the Inode linked list and display:
                            - File Name
                            - Inode Number
                            - Actual File Size
                            - Link Count
                    Step 4: If the FileType of an inode is 0 then it is considered "free" and is skipped.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


void ls_file()
{
    int i = 0;
    PINODE temp = head;               // Start from the beginning of the Inode linked list

    // If all inodes are free, that means no files have been created
    if(SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("ERROR : There are no files\n");
        return;
    }

    printf("\nFile Name\tInode Number\tFile Size\tLink Count\n");
    printf("-------------------------------------------------------------\n");
    while(temp != NULL)                                                              // LinkedList inodes traverse until it gets null
    {
        if(temp->FileType != 0)         // FileType such as Regular File Type - 1 & Special File Type - 2 (Active Files means not deleted files).
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("---------------------------------------------------------------\n");
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                         [fstat_file()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : fstat_file
    Description   : Displays statistical information of a file in the Customized Virtual File System (CVFS)
                    by using its File Descriptor (FD).
                    It retrieves the information from the inode structure.

    Parameters    : 1] fd : The File Descriptor of the file for which information is to be displayed.
                    
    Return Value  : return 0 if the information is successfully displayed.
                    return -1 if the file descriptor is invalid (less than 0).
                    return -2 if the file descriptor does not point to an active inode.

    Logic         : Step 1: Validate the file descriptor.
                    Step 2: Ensure the file table for the given FD is not NULL.
                    Step 3: Access the inode associated with the file table.
                    Step 4: Print the following details:
                    - File Name
                    - Inode Number
                    - File Size (maximum size)
                    - Actual File Size (used space)
                    - Link Count
                    - Reference Count
                    - File Permission (Read, Write, or Read & Write)
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;

    if(fd < 0)      return -1;                                      // Invalid FD

    if(UFDTArr[fd].ptrfiletable->ptrinode == NULL)  return -2;      // File not present

    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n--------Statistical Information about file--------\n");
    printf("File name        : %s\n",temp->FileName);
    printf("Inode Number     : %d\n",temp->InodeNumber);
    printf("File size        : %d\n",temp->FileSize);
    printf("Actual File size : %d\n",temp->FileActualSize);
    printf("Link count       : %d\n",temp->LinkCount);
    printf("Reference count  : %d\n",temp->RefrenceCount);

    if(temp->Permission == 1)
        printf("File Permission : Read only\n");
    else if(temp->Permission == 2)
        printf("File Permission : Write\n");
    else if(temp->Permission == 3)
        printf("File Permission : Read & Write\n");
    printf("----------------------------------------------------\n\n");

    return 0;
}
 

//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                         [stat_file()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : stat_file
    Description   : Displays statistical information of a file in the Customized Virtual File System (CVFS)
                    by using its file name. It retrieves the information from the inode structure.

    Parameters    : 1] name - The name of the file for which information is to be displayed.
        
    Return Value  : return 0 if the information is successfully displayed.
                    return -1 if the file name is NULL (incorrect parameter).
                    return -2 if the file is not found in the Inode List.
        
    Logic         :
                    Step 1: Validate the parameter (file name).
                    Step 2: Traverse the inode linked list to find the specified file.
                    Step 3: If the file is not found return -2.
                    Step 4: If the file is found then display : 
                                                                - File Name
                                                                - Inode Number
                                                                - Maximum File Size
                                                                - Actual File Size (used space)
                                                                - Link Count
                                                                - Reference Count
                                                                - File Permission (Read, Write, or Read & Write)
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//


int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)    return -1;              // Invalid parameter

    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName) == 0)
            break;
        temp = temp->next;
    }
    
    if(temp == NULL)    return -2;              // file not found

    printf("\n---------Statistical Information about file----------\n");
    printf("File name        : %s\n",temp->FileName);
    printf("Inode Number     : %d\n",temp->InodeNumber);
    printf("File size        : %d\n",temp->FileSize);
    printf("Actual File size : %d\n",temp->FileActualSize);
    printf("Link count       : %d\n",temp->LinkCount);
    printf("Reference count  : %d\n",temp->RefrenceCount);

    if(temp->Permission == 1)
        printf("File Permission : Read only\n");
    else if(temp->Permission == 2)
        printf("File Permission : Write\n");
    else if(temp->Permission == 3)
        printf("File Permission : Read & Write\n");
    printf("-----------------------------------------------------\n\n");

    return 0;
}


//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                         [truncate_file()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
//      what is memset?
//      memset is a standard C library function used to set a block of memory to a specific value.
//      void *memset(void *ptr, int value, size_t num);
//      ptr → Pointer to the block of memory to fill.
//      value → The value to set (converted to unsigned char).
//      num → Number of bytes to be set.
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : truncate_File
    Description   : Removes all data from the specified file in the Customized Virtual File System (CVFS).
                    It clears the contents of the file buffer and resets the offsets 
                    and updates the file's actual size to zero.

    Parameters    : 1] name : The name of the file to be truncated (all its content will be removed).
       
    Return Value  : return 0 if the file is successfully truncated.
                    return -1 if the file is not found in the User File Descriptor Table (UFDT).
       
    Logic         : Step 1: Get the File Descriptor (FD) using the file name.
                    Step 2: If the file is not found then return -1.
                    Step 3: Use memset to clear the entire buffer (set it to 0).
                    Step 4: Reset the readoffset and writeoffset to the beginning of the file.
                    Step 5: Set the FileActualSize to 0 that indicating it is empty.
 */
//--------------------------------------------------------------------------------------------------------------------------------------------//

int truncate_File(char *name)
{
    int fd = 0;
    fd = GetFDFromName(name);
    if(fd == -1)
        return -1;                                                  // file not found

    // Clears the contents in the buffer of a file
    // This sets all 1024 bytes in the buffer to 0 (like wiping the data)
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;

    return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------//
//                                                         [main()]
//--------------------------------------------------------------------------------------------------------------------------------------------//
/*
    Function Name : main
    Description   : This is the entry point for the Customized Virtual File System (CVFS).
                   It initializes core components like the SuperBlock and Disk Inode List Block (DILB).
                   It then enters an infinite loop to accept commands from the user for file management operations like:
                            - Creating Files
                            - Reading/Writing Files
                            - Listing Files
                            - Closing Files
                            - Deleting Files
                            - Showing File Statistics
                            - Displaying Help
                            - Exiting the system

    Parameters    : None
    Return Value  : return 0 when the CVFS is terminated using the exit command.

    Logic         :  Step 1: Initialize the SuperBlock and the Disk Inode List Block (DILB).
                     Step 2: Start an infinite loop to accept commands from the user.
                     Step 3: Read user input and parse it into command words.
                     Step 4: Execute different commands based on user input:

            1]ls        : Lists all the files present in the CVFS with details like File Name, Inode Number, File Size, and Link Count.
            2]closeall  : Closes all currently opened files and resets their offsets.
            3]clear     : Clears the terminal screen.
            4]help      : Displays a list of available commands with descriptions.
            5]exit      : Terminates the Customized Virtual File System (CVFS) safely.
            6]stat      : Displays the statistical information of a specified file using its name.
            7]fstat     : Displays the statistical information of a specified file using its File Descriptor (FD).
            8]close     : Closes a specific file that is currently open using its name.
            9]rm        : Removes (deletes) a specified file from the CVFS.
            10]man      : Displays the manual (help) of a specific command with usage.
            11]write    : Writes data to a specified file from the CVFS.
            12]truncate : Removes all data from a specified file, setting its size to 0.
            13]create   : Creates a new regular file in the CVFS with specified permissions.
            14]open     : Opens a specified file with the given mode (Read, Write, or Read + Write).
            15]read     : Reads the content of a specified file and displays it on the screen.
            16]lseek    : Repositions the read/write offset of an open file to a specified location.
            17]Command not found !!!
 */

//--------------------------------------------------------------------------------------------------------------------------------------------//


int main()
{
    char *ptr = NULL;                           // Pointer for memory allocation if needed
    int ret = 0, fd = 0, count = 0;             // Return values and counters
    char command[4][80], str[80], arr[1024];    // Buffers for commands and data

    InitialiseSuperBlock();
    CreateDILB();

    while(1)
    {
        fflush(stdin);                      // Clears stdin buffer
        strcpy(str,"");                     // Clears old input string

        printf("\nCustomized VFS : > ");   // Prompts user for a command

        fgets(str,80,stdin);                // scanf("%[^'\n']s",str);      // Accept command from the user
    
        count = sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);  // sscanf stands for string scan formatted. It is used to extract data from a string, similar to how scanf reads input from the user — but instead of reading from the keyboard, it reads from a given string.

            if(count == 1)
            {
                if(strcmp(command[0],"ls") == 0)                // strcmp is string compare which is used to compare two strings. If 0 then strings are equal. If <0 then first string is less than second. If >0 then first string is greater than second.
                {
                    ls_file();                                  // Calls the function that lists all existing files in the customized virtual file system.
                    continue;                                   // continue will exit the current loop and again start from new loop
                }   
                else if(strcmp(command[0],"closeall") == 0)    
                {
                    CloseAllFile();                                     // {*******Currentlty close one file only should be modified.}
                    printf("All files closed successfully\n");
                    continue;
                }
                else if(strcmp(command[0],"clear") == 0)
                {
                    system("cls");                                  // Used system call directly to execute command cls is only used in Windows not on Unix nor Linux.
                    continue;
                }
                else if(strcmp(command[0],"help") == 0)
                {
                    DisplayHelp();                                // Provides info about what command does 
                    continue;
                }
                else if(strcmp(command[0],"exit") == 0)
                {
                    printf("Terminating the Customized Virtual File System...\n");         // Terminates the program
                    break;
                }
                else 
                {
                    printf("\nERROR : Command not found !!!\n");
                    continue;
                }
            }
            else if(count == 2)
            {
                if(strcmp(command[0],"stat") == 0)
                {
                    ret = stat_file(command[1]);                        //  command[1] - the second word as a argument which is name of the file.
                    if(ret == -1)
                        printf("ERROR : Incorrect parameters\n");
                    if(ret == -2)
                        printf("ERROR : There is no such file\n");
                    continue;
                }
                else if(strcmp(command[0],"fstat") == 0)        
                {
                    ret = fstat_file(atoi (command[1]));                   // command[1] - the second word as an argument (typically the file descriptor or index in the UFDT array).
                                                                           // atoi - If fstat_file() function expects an integer file descriptor then need to convert the command[1] string to an integer using atoi() - [converts string to integer]
                    if(ret == -1)
                        printf("ERROR : Incorrect parameters\n");
                    if(ret == -2)
                        printf("ERROR : There is no such file\n");
                    continue;
                }
                else if(strcmp(command[0],"close") == 0)
                {
                    ret = CloseFileByName(command[1]);

                    if(ret == -1)
                        printf("ERROR : There is no such file\n");
                    continue;
                }
                else if(strcmp(command[0],"rm") == 0)
                {
                    ret = rm_file(command[1]);

                    if(ret == -1)
                        printf("ERROR : There is no such file\n");
                    continue;
                }
                else if(strcmp(command[0],"man") == 0)
                {
                    man(command[1]);
                }
                else if(strcmp(command[0],"write") == 0)
                {
                    fd = GetFDFromName(command[1]);
                    if(fd == -1)
                    {
                        printf("Incorrect parameter\n");
                        continue;
                    }
                    char ch;
                    scanf("%[^'\n']s",arr);
                    scanf("%c",&ch);

                    ret = strlen(arr);
                    if(ret == 0)
                    {
                        printf("Incorrect parameter\n");
                        continue;
                    }
                    ret = WriteFile(fd,arr,ret);
                    if(ret == -1)
                        printf("ERROR : Permission denied\n");
                    if(ret == -2)
                        printf("ERROR : There is no sufficient memory to write\n");
                    if(ret == -3)
                        printf("ERROR : It is not regular file\n");
                }
                else if(strcmp(command[0],"truncate") == 0)
                {
                    ret = truncate_File(command[1]);
                    if(ret == -1)
                        printf("Error : Incorrect parameter\n");
                }
                else
                {
                    printf("ERROR : Command not found !!!\n");
                        continue;
                }
            }
            else if(count == 3)
            {
                if(strcmp(command[0],"create") == 0)
                {
                    ret = CreateFile(command[1],atoi (command[2]));
                    if(ret >= 0)
                        printf("File is successfully created with file descriptor : %d\n",ret);
                    if(ret == -1)
                        printf("ERROR : Incorrect parameters\n");
                    if(ret == -2)
                        printf("ERROR : There are no inodes available\n");
                    if(ret == -3)
                        printf("ERROR : File already exists\n");
                    if(ret == -4)
                        printf("ERROR : Memory allocation failure\n");
                    continue;
                }
                else if(strcmp(command[0],"open") == 0)
                {
                    ret = OpenFile(command[1],atoi(command[2]));
                    if(ret >= 0)
                        printf("File is successfully opened with file descriptor : %d\n",ret);
                    if(ret == -1)
                        printf("ERROR : Incorrect parameters\n");
                    if(ret == -2)
                        printf("ERROR : File not present\n");
                    if(ret == -3)
                        printf("ERROR : Permission denied\n");
                    if(ret == -4)
                        printf("Memory allocation failure\n");
                    continue;
                }
                else if(strcmp(command[0],"read") == 0)
                {
                    fd = GetFDFromName(command[1]);
                    if(fd == -1)
                    {
                        printf("Error : Incorrect parameter\n");
                        continue;
                    }
                    ptr = (char*)malloc(sizeof(atoi(command[2]))+1);
                    if(ptr == NULL)
                    {
                        printf("Error : Memeory allocation failure\n");
                        continue;
                    }
                    ret = ReadFile(fd,ptr,atoi(command[2])); 
                    if(ret == -1)
                        printf("ERROR : File not existing\n");
                    if(ret == -2)
                        printf("ERROR : Permission denied\n");
                    if(ret == -3)
                        printf("ERROR : Reached at end of file\n");
                    if(ret == -4)
                        printf("ERROR : It is not regular file\n");
                    if(ret == 0)
                        printf("ERROR : File empty\n");
                    if(ret > 0)
                    {
                        write(2,ptr,ret);                               // here 2 is the standard error output which is inside the UFDTArr.
                    }
                    continue;
                }
                else 
                {
                    printf("\n ERROR : Command not found!!!\n");
                    continue;
                }
            }
            else if(count == 4)
            {
                if(strcmp(command[0],"lseek") == 0)
                {
                    fd = GetFDFromName(command[1]);
                    if(fd == -1)
                    {
                        printf("Error : Incorrect parameter\n");
                        continue;
                    }
                    ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
                    if(ret == -1)
                    {
                        printf("ERROR : Unable to perform lseek\n");
                    }
                }
                else
                {
                    printf("\nERROR : Command not found !!!\n");
                    continue;
                }
            }
            else 
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }
    }
    return 0;
}
