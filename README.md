
# 🗂️ Customised Virtual File System (CVFS)

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Cross--Platform-green.svg)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen.svg)

**Technology:** C Programming  

---

## 📌 Project Overview  
This project is a **custom implementation of a Virtual File System (VFS)** that simulates the core functionality of the Linux file system.  
It is built entirely in **C**, with its own **custom shell** to interact with the virtual environment.  

The project provides a **practical understanding of system calls, file handling, memory management, and OS internals**.

---

## ✨ Key Features  

- **Custom Shell Interface**  
  Linux-like commands for file operations: `create`, `open`, `read`, `write`, `delete`, `ls`, etc.  

- **System Call Simulation**  
  Implements Linux-like system calls: `open`, `read`, `write`, `lseek`, `close`, `rm`, etc.  

- **File System Data Structures**  
  - Incore Inode Table  
  - File Table  
  - UAREA (User Area)  
  - User File Descriptor Table  

- **Platform Independent**  
  System-level file handling functionalities of Linux simulated on any OS.  

- **Database-like Functionality**  
  Customised database management layer with structured file handling.  

---

## 🎯 Learning Outcomes  

- Deep understanding of **Linux File System internals**.  
- Practical knowledge of OS data structures (inode, file tables, UAREA).  
- Strong grasp of **system programming in C**.  
- Hands-on experience with **shell design & command interpreter**.  
- Application of **low-level OS-like logic building**.  

---

## 🚀 Example Usage  

```bash
gcc CVFS.cpp -o Myexe
Myexe.exe (Windows)
./Myexe    (Linux)

Customized CVFS > create Demo.txt
Customized CVFS > write Demo.txt "Jay Ganesh"
Customized CVFS > read Demo.txt
Jay Ganesh
Customized CVFS > ls
Demo.txt
Customized CVFS > rm Demo.txt
Customized CVFS > exit
