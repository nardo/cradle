// Cradle Engine example master server - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

void processConfigLine(int argc, const char **argv)
{
   if(!strcasecmp(argv[0], "port") && argc > 1)
   {
      M.master_port = atoi(argv[1]);
   }
   else if(!strcasecmp(argv[0], "motd") && argc > 2)
   {
      char *type = strdup(argv[1]);
      char *message = strdup(argv[2]);

      M.MOTDTypeVec.push_back(type);
      M.MOTDStringVec.push_back(message);
   }
}

enum {
   MaxArgc = 128,
   MaxArgLen = 100,
};

static char *argv[MaxArgc];
static char argv_buffer[MaxArgc][MaxArgLen];
static int argc;
static int argLen = 0;
static const char *argString;

inline char getNextChar()
{
   while(*argString == '\r')
      argString++;
   return *argString++;
}

inline void addCharToArg(char c)
{
   if(argc < MaxArgc && argLen < MaxArgLen-1)
   {
      argv[argc][argLen] = c;
      argLen++;
   }
}

inline void addArg()
{
   if(argc < MaxArgc)
   {
      argv[argc][argLen] = 0;
      argc++;
      argLen = 0;
   }
}

int parseArgs(const char *string)
{
   int numObjects = 0;

   argc = 0;
   argLen = 0;
   argString = string;
   char c;

   for(uint32 i = 0; i < MaxArgc; i++)
      argv[i] = argv_buffer[i];

stateEatingWhitespace:
   c = getNextChar();
   if(c == ' ' || c == '\t')
      goto stateEatingWhitespace;
   if(c == '\n' || !c)
      goto stateLineParseDone;
   if(c == '\"')
      goto stateReadString;
   if(c == '#')
      goto stateEatingComment;
stateAddCharToIdent:
   addCharToArg(c);
   c = getNextChar();
   if(c == ' ' || c == '\t')
   {
      addArg();
      goto stateEatingWhitespace;
   }
   if(c == '\n' || !c)
   {
      addArg();
      goto stateLineParseDone;
   }
   if(c == '\"')
   {
      addArg();
      goto stateReadString;
   }
   goto stateAddCharToIdent;
stateReadString:
   c = getNextChar();
   if(c == '\"')
   {
      addArg();
      goto stateEatingWhitespace;
   }
   if(c == '\n' || !c)
   {
      addArg();
      goto stateLineParseDone;
   }
   if(c == '\\')
   {
      c = getNextChar();
      if(c == 'n')
      {
         addCharToArg('\n');
         goto stateReadString;
      }
      if(c == 't')
      {
         addCharToArg('\t');
         goto stateReadString;
      }
      if(c == '\\')
      {
         addCharToArg('\\');
         goto stateReadString;
      }
      if(c == '\n' || !c)
      {
         addArg();
         goto stateLineParseDone;
      }
   }
   addCharToArg(c);
   goto stateReadString;
stateEatingComment:
   c = getNextChar();
   if(c != '\n' && c)
      goto stateEatingComment;
stateLineParseDone:
   if(argc)
      processConfigLine(argc, (const char **) argv);
   argc = 0;
   argLen = 0;
   if(c)
      goto stateEatingWhitespace;
   return numObjects;
}      

void readConfigFile()
{
   FILE *f = fopen("./master.cfg", "r");
   if(!f)
   {
      printf("Unable to open config file.");
      return;
   }
   for(uint32 i = 0; i < M.MOTDTypeVec.size();i++)
   {
      free(M.MOTDTypeVec[i]);
      free(M.MOTDStringVec[i]);
   }
   M.MOTDTypeVec.clear();
   M.MOTDStringVec.clear();

   char fileData[32768];

   size_t bytesRead = fread(fileData, 1, sizeof(fileData), f);
   fileData[bytesRead] = 0;

   parseArgs(fileData);

   fclose(f);
}

