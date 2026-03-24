# MiniGit

MiniGit is a lightweight, simplified implementation of the Git version control system written entirely in C.
## Features

MiniGit successfully replicates the core storage and tracking mechanics of real Git:
- **Object Database**: Hashes files using SHA-1 and compresses them using `zlib`.
- **Blob, Tree, and Commit Objects**: Correctly formats and stores different Git object types.
- **Staging Area**: Maintains an `.index` file to track tracked and staged files.
- **History Traversal**: Parses and decompresses binary commit objects to trace repository history.

## Dependencies

To compile and run MiniGit, your system must have the standard C build tools, as well as the OpenSSL and zlib development headers.

**On Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential libssl-dev zlib1g-dev
```
**On MacOS:**
```
brew install openssl zlib
```

## **Project Structure**

```
minigit/
├── Makefile
├── README.md
├── include/
│   ├── commit.h
│   ├── index.h
│   ├── init.h
│   ├── log.h
│   ├── objects.h
│   └── status.h
└── src/
    ├── commit.c    # Logic for generating trees and commits
    ├── index.c     # Logic for parsing and updating the staging area
    ├── init.c      # Directory and branch initialization
    ├── log.c       # History traversal and object decompression
    ├── main.c      # CLI argument parsing and routing
    ├── objects.c   # Core engine for SHA-1 hashing and Zlib compression
    └── status.c    # Formatted output for the current index state
```


## Usage:
 * #### init:
 	Initializes a new, empty repository. Creates the hidden .minigit directory 			structure and the default master branch pointer.
 	```
 	./minigit init
 	```
 	
* #### add:
	Stages a file for the next commit. MiniGit reads the file, compresses it into 		a blob object, saves it in .minigit/objects/, and records the file path and 		hash in .minigit/index.
	```
	./minigit add <filename>
	```
* #### status:
	Reads the staging area (.minigit/index) and displays the files that are 			currently staged and ready to be committed.

```
./minigit status
```

* #### commit:

	Takes a snapshot of the staging area. It generates a Tree object representing 		the 	directory structure, creates a Commit object linking to the Tree and the 		parent commit, and advances the master branch pointer.

```
./minigit commit -m "Your commit message here"
```

* #### log:

	Traverses the commit history. It starts at the HEAD pointer, uncompresses the 		commit objects on the fly, and prints the author, date, message, and commit 		hash, traversing backwards through the parent hashes.
```
./minigit log
```
