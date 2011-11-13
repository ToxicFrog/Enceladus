// The TOC consists of any number of entries, followed by a footer
// The footer is the number of entries (size_t) followed by the magic
// number (uint32_t)
// an entry is:
// struct { size_t offset; size_t namesize; size_t filesize; }
// and the corresponding data is
// char[namesize] filename; uint8_t[filesize] data;

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>

#include "io.h"

const uint32_t MAGIC_NUMBER = 0x454E4345; // 'ENCE'

typedef struct {
    size_t offset;
    size_t namesize;
    size_t filesize;
} TOCEntry;

// load the TOC from the end of the named file
// pushes two items onto the stack
// the first is a table of path -> contents mappings for the packed files
// the second is the name of the packed file containing the entry point
int readTOC(lua_State * L, const char * name)
{
    FILE * fin = fopen(name, "rb");
    
    // look for the Enceladus magic number at the end of the executable
    uint32_t magic;
    fseek(fin, -4, SEEK_END);
    fread(&magic, sizeof(uint32_t), 1, fin);
    if (magic != MAGIC_NUMBER)
    {
        fclose(fin);
        return 0;
    }
    
    // we found it; read the TOC
    size_t nrof_files;
    fseek(fin, -4 - sizeof(size_t), SEEK_END);
    fread(&nrof_files, sizeof(size_t), 1, fin);
    
    TOCEntry * TOC = malloc(sizeof(TOCEntry) * nrof_files);
    fseek(fin, -4 - sizeof(size_t) - sizeof(TOCEntry) * nrof_files, SEEK_END);
    fread(TOC, sizeof(TOCEntry), nrof_files, fin);
    
    lua_newtable(L);
    for (int i = 0; i < nrof_files; ++i)
    {
        char * name = loadslice(fin, TOC[i].offset, TOC[i].namesize);
        char * source = loadslice(fin, TOC[i].offset + TOC[i].namesize, TOC[i].filesize);
        
        // file name -> compiled function
        // we don't check the RV of luaL_loadbuffer because the files were
        // checked for compile-time errors at embed time
        lua_pushlstring(L, name, TOC[i].namesize);
        luaL_loadbuffer(L, source, TOC[i].filesize, name);
        lua_settable(L, -3);
        
        free(name);
        free(source);
    }
    
    // the first entry in the TOC is always main, so push the name of that
    // so that we can retrieve it
    char * mainname = loadslice(fin, TOC[0].offset, TOC[0].namesize);
    lua_pushlstring(L, mainname, TOC[0].namesize);
    free(mainname);
    
    free(TOC);
    fclose(fin);
    
    return 1;
}

int writeTOC(lua_State * L, const char * name, size_t nrof_files, char ** files)
{
    TOCEntry * TOC = malloc(sizeof(TOCEntry) * nrof_files);
    FILE * fout = fopen(name, "ab");
    if (!fout)
    {
        perror("writeTOC");
        return 0;
    }
    
    for (int i = 0; i < nrof_files; ++i)
    {
        TOC[i].offset = ftell(fout);
        TOC[i].namesize = strlen(files[i]);
        uint8_t * buf = loadfile(files[i], &(TOC[i].filesize));
        if (!buf)
        {
            perror("writeTOC");
            fclose(fout);
            remove(name);
            return 0;
        }
        
        // test to make sure that all of the files are syntactically valid
        if (luaL_loadbuffer(L, buf, TOC[i].filesize, files[i]) != 0)
        {
            fprintf(stderr, "error embedding: %s\n", lua_tostring(L, -1));
            fclose(fout);
            remove(name);
            return 0;
        }
        
        fwrite(files[i], TOC[i].namesize, 1, fout);
        fwrite(buf, TOC[i].filesize, 1, fout);
        free(buf);
    }
    
    fwrite(TOC, sizeof(TOCEntry), nrof_files, fout);
    fwrite(&nrof_files, sizeof(size_t), 1, fout);
    fwrite(&MAGIC_NUMBER, sizeof(uint32_t), 1, fout);
    fclose(fout);
    
    return 1;
}
