// File header uses ints for booleans
typedef int BOOL;

#define PATH_LENGTH 1024
#define PROP_STRING_SIZE 128
#define PROP_LONG_STRING_SIZE 1024

// File Header:
struct BytecodeHeader
{
    // File header
    // Can be one of the following:
    // - "TWEST": Valid executable file
    // - "ERROR": Invalid file
    char file_header[6];
    
    // TProlog Preferences
    BOOL closeWindowOnTerminate;    // Whether to close the window on program end
    BOOL runWithArgs;               // Whether to show the "Run with Arguments" dialouge
    BOOL centerWindow;              // Whether to center the output window
    BOOL dontTerminate;             // If the user shouldn't be able to abort the program
    
    // General Properties (from edprog.h)
    struct
    {
        BOOL	propertiesInitialized;
        
        // Admin section
        char	startupDirectory[PATH_LENGTH];
        BOOL	useMyDocuments;
        BOOL	studentsCanChangeUserName;
        BOOL	studentCanEnterAdvancedMode;
        BOOL	studentRestrictedToStartupDir;
        BOOL	studentDrivesAllowed;
        char	studentDrivesAllowedList [27];
        BOOL	studentDrivesForbidden;
        char	studentDrivesForbiddenList [27];
        BOOL	studentCantChangePrefs;
        
        // General section
        BOOL	beginnerMode;
        BOOL	oneWindowMode;
        BOOL	useFullPathNames;
        BOOL	editSaveBackup;
        int		editIndentSize;
        BOOL	confirmQuit;
        BOOL	skipSplashScreen;
        BOOL	addSuffixToFilesWithNone;
        BOOL	editLeadingSpacesToTabs;
        BOOL	editOpenBraceOnSameLine;
        int		editNumSpacesPerTab;
        BOOL	noRecentFiles;
        BOOL	useLastDirectory;
        BOOL	findUsesSelection;
        BOOL	displayDebuggerMenu;
        BOOL	displayWindowMenu;
            
        // Appearance section
        char	editFontName [PROP_STRING_SIZE];
        int		editFontSize;
        BOOL	editSyntaxColouring;
        BOOL	editBeginnerFullScreen;
        BOOL	editAdvancedFullScreen;
        int		editTextRows;
        int		editTextCols;
        // editCaretWidth: 1 = 1 pixel wide, 2 = 2 pixel wide, 3 = char wide
        int		editCaretWidth;
        BOOL	debuggerMenu;
        BOOL	windowMenu;
        
        // Run section
        BOOL	useGraphicsMode;
        char	runConsoleFontName [PROP_STRING_SIZE];
        int		runConsoleFontSize;
        BOOL	runConsoleFullScreen;
        int		runConsoleTextRows;
        int		runConsoleTextCols;
        int		runAppletWidth;
        int		runAppletHeight;
        BOOL	runUseSmallFont;
        
        // Java section
        char	additionalClassPath [PROP_LONG_STRING_SIZE];
        char	jikesOptions [PROP_LONG_STRING_SIZE];
        BOOL	startJavaRunnerInDebugger;
        int		memoryForJVMInMB;
        BOOL	oldJavaCompile;	// TRUE = Target Java 1.3 = No asserts
        int		JVMType;	// Only partially supported
        char	versionInRegistry [20];
        
        // Turing section
        // Minimum stack size is 64KiB, max is 8192KiB
        int		turingStackSizeInKB;
        int		executionDelay;
        int		parallelIOPort;
        BOOL	prohibitSysExec;
        BOOL	noSound;
    
        // Print section
        char	printFontName [PROP_STRING_SIZE];
        char	userName [PROP_STRING_SIZE];
        int		printFontSize;
        BOOL	printBoldKeywords;
        BOOL	printItalicizeIdents;
        BOOL	printPageHeader;
        BOOL	printUserName;
        BOOL	printLineNumbers;
        char	printHeaderFontName [PROP_STRING_SIZE];
        int		printHeaderFontSize;
        BOOL	printBorder;
        int		printMarginLeft, printMarginRight;
        int		printMarginTop, printMarginBottom;
        BOOL	printTwoUp;
        
        // Special section
        int		numRecentFiles;			// Not in dialog
        int		editOneWindowMargin;		// Not in dialog
        BOOL	catchExceptions;		// Not in dialog
        BOOL	debuggerAvailable;		// Not in dialog
        
        // Debug section
        BOOL	debug;			// Verifies line DB
        BOOL	debugWindowAreas;	// Colours window areas
        BOOL	logging;		// Writes a log
        BOOL	logWindowMessages;	// Logs messages to windows
        // Normally Turing maps stream 0 to stdout instead of stderr!
        BOOL	stream0ToStderr;	// Output to stream 0 to stderr
        BOOL	errorsToStderr;		// Library errors to stderr
        BOOL	infoToStderr;		// Library info to stderr
    } Properties;
    
    char header_sync[6];
};

struct FileTableHeader
{
    short fileNo;         // normalFN
    char filename[256]; // FileName+Path
    short bodyFileNo;   // bodyFN
    short stubFileNo;   // stubFN
    
    // Code Table (Bytecode Area)
    int codeTableSize;
    char* codeTable;
    
    // Manifest Table (Read-only Data)
    int manifestTableSize;
    char* manifestTable;
    
    // Globals (Runtime Data)
    int globalSize;
    char* globalArea;
    
    /** Things which are technically aren't part of the memory structure, but are part of the file structure **/
    // Table modification
    union {
        int manifest_copy; // If 0xFFFFFFFF, don't do anything
        
        // Represents an offset patch to modifiy the manifest with. Always 4 bytes at a time
        struct {
            int manifest_offset;        // Area of the current manifest table to modify (in multiples of 4 bytes)
            short fileNo;               // File to get the table offsets from
            int table;                  // Which table from the external file to use as a base address
                                        //  0 = Manifest
                                        //  1 = Code
                                        //  2 = Global
            int table_offset;           // Offset into the source table to apply a patch
        };
        
        // Patches are applied in this manner:
        // *(this->manifestTable + manifest_offset) = tableOf(fileNo, table).base + table_offset
    };
    
    /// Modifiy sections of code using data from other tables    ///
    /// Done so that the code is position independent            ///
    
    
    // List of patches with tables in the same file.
    // Linked list of patches, terminated with NULL
    // Patch structure:
    /*
    struct {
        int next;       // CodeTable offset to next location
        int offset;     // Offset into requested table
    } Patch;
    */
    // Requested table changes depending on context
    
    // Patch heads
    // Adds on the offset of the code area
    int codePatchHead;
    // Adds on the offset of the manifest area
    int manifestPatchHead;
    // Adds on the offset of the global area
    int globalPatchHead;
    
    
    // Patch location:
    // Det by:
    // patch_location = code_table + tablePatch + 4
    // *patch_location += tableArea
    // tablePatch = *(patch_location - 4)
    // Where:
    //  - tablePatch : Current table patch offset
    //  - tableArea  : Current table to add the offset onto
    
    // List of patches with tables of differing files
    // Begins with a file specifier, which contains the appropriate patch head:
    
    struct {
        short fileNo;   // File to read from. 0xFFFF terminates the list
        int patchHead;  // Start of the respective table's patch set
    } FilePatchset;
    
    // There are 3 patch set lists (in order of appearence)
    //  - Code patch sets
    //  - Manifest patch sets
    //  - Global patch sets
    // Each patch set has its own lists of file selections, leading to the following structure:
    // CodePatchSet
    //  \ EOL (end of list)
    // ManifestPatchSet
    //  \ File 0, off 20
    //  \ File 18, of 64
    //  \ EOL
    // GlobalPatchSet
    //  \ File 3, off 8
    //  \ File 7, off 0
    //  \ EOL
    
    FilePatchset codePatchSet;      // For calling external functions
    FilePatchset manifestPatchSet;  // For accessing external static data
    FilePatchset globalPatchSet;    // For accessing external dynamic data
    
    // Each patch set list is terminated with a 0xFFFF after all of the patches
};

/**
General file structure information:

BytecodeHeader: Contains information about the execution environment
FileTable[]: List of all of the files
    - FileTableEntry: One entry into the table of files
        \ fileNo: File number of the file
        \ filename: Path to the source file
        \ bodyFileNo: Unknown usage
        \ stubFileNo: Unknown usage
        \ codeTable: Contains the actual code
        \ manifestTable: Contains read-only / initialization data
        \ globalArea: Contains read-write data
**/