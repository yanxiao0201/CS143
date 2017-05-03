/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"


using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  BTreeIndex tree;
    IndexCursor cursor;

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;
    
  rid.pid = rid.sid = 0;
  count = 0;
    

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

    int keyquery = 0;
    int minkey = -1;
    int maxkey = -1;
    int equalkey = -1;
    
    SelCond tmp;
    int valueInput = 0;
    bool useindex = false;
    
    
    for (unsigned j = 0; j < cond.size(); j++){
        tmp = cond[j];
        int tmpval = atoi(tmp.value);
    
        if (tmp.attr == 1 && tmp.comp != SelCond::NE){
            keyquery ++;
            if (tmp.comp == SelCond::EQ){
                equalkey = tmpval;
                break;
            }
            else if (tmp.comp == SelCond::LT){
                if(tmpval <= maxkey || maxkey == -1){
                    maxkey = tmpval - 1 ;
                }
 
            }
            else if (tmp.comp == SelCond::LE){
                if(tmpval <= maxkey || maxkey == -1){
                    maxkey = tmpval;
                }
            }
            else if (tmp.comp == SelCond::GT){
                if(tmpval >= minkey || minkey == -1){
                    minkey = tmpval + 1;
                }
            }
            else if (tmp.comp == SelCond::GE){
                if(tmpval >= minkey || minkey == -1){
                    minkey = tmpval + 1;
                }
            }
        }
        else if (tmp.attr == 2){
            valueInput++;
        }
    }
    
    if (maxkey!=-1 && minkey!=-1 && maxkey<minkey){
        goto end_if_unreasonable;
    }
    


  if ((rc = tree.open(table + ".idx",'r')) != 0 || (keyquery == 0 && attr !=4)){
    
      while (rid < rf.endRid()) {
          // read the tuple
          if ((rc = rf.read(rid, key, value)) < 0) {
              fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
              goto exit_select;
          }
          
          // check the conditions on the tuple
          for (unsigned i = 0; i < cond.size(); i++) {
              // compute the difference between the tuple value and the condition value
              switch (cond[i].attr) {
                  case 1:
                      diff = key - atoi(cond[i].value);
                      break;
                  case 2:
                      diff = strcmp(value.c_str(), cond[i].value);
                      break;
              }
              
              // skip the tuple if any condition is not met
              switch (cond[i].comp) {
                  case SelCond::EQ:
                      if (diff != 0) goto next_tuple;
                      break;
                  case SelCond::NE:
                      if (diff == 0) goto next_tuple;
                      break;
                  case SelCond::GT:
                      if (diff <= 0) goto next_tuple;
                      break;
                  case SelCond::LT:
                      if (diff >= 0) goto next_tuple;
                      break;
                  case SelCond::GE:
                      if (diff < 0) goto next_tuple;
                      break;
                  case SelCond::LE:
                      if (diff > 0) goto next_tuple;
                      break;
              }
          }
          
          // the condition is met for the tuple. 
          // increase matching tuple counter
          count++; 
          
          // print the tuple 
          switch (attr) {
              case 1:  // SELECT key
                  fprintf(stdout, "%d\n", key);
                  break;
              case 2:  // SELECT value
                  fprintf(stdout, "%s\n", value.c_str());
                  break;
              case 3:  // SELECT *
                  fprintf(stdout, "%d '%s'\n", key, value.c_str());
                  break;
          }
          
          // move to the next tuple
      next_tuple:
          ++rid;
      }
    }
    
  else{
      useindex = true;
      
      if(equalkey != -1){
          tree.locate(equalkey,cursor);
      }
      else if(minkey != -1){
          tree.locate(minkey,cursor);
      }
      else{
          tree.locate(0,cursor);
      }
    
      
      while(tree.readForward(cursor, key, rid) == 0){
          if (valueInput == 0 && attr == 4){
              count++;
              continue;
          }
          
          rc = rf.read(rid, key, value);
          if (rc < 0){
              fprintf(stderr, "Error reading tuple from %s",table.c_str());
              goto exit_select;
          }
          
          for (unsigned i = 0; i < cond.size(); i++) {
              // compute the difference between the tuple value and the condition value
              switch (cond[i].attr) {
                  case 1:
                      diff = key - atoi(cond[i].value);
                      break;
                  case 2:
                      diff = strcmp(value.c_str(), cond[i].value);
                      break;
              }
              
              switch (cond[i].comp) {
                  case SelCond::EQ:
                      if (diff != 0){
                          if (cond[i].attr == 1){
                              goto end_if_unreasonable;
                          }
                          else{
                              goto next_tuple2;
                          }
                      }
                      break;
                  case SelCond::NE:
                      if (diff == 0) goto next_tuple2;
                      break;
                  case SelCond::GT:
                      if (diff <= 0) goto next_tuple2;
                      break;
                  case SelCond::LT:
                      if (diff >= 0)
                      {
                          if (cond[i].attr == 1){
                              goto end_if_unreasonable;
                          }
                          else{
                              goto next_tuple2;
                          }
                      }
                      break;
                  case SelCond::GE:
                      if (diff < 0) goto next_tuple2;
                      break;
                  case SelCond::LE:
                      if (diff > 0){
                          if (cond[i].attr == 1){
                              goto end_if_unreasonable;
                          }
                          else{
                              goto next_tuple2;
                          }
                      }
                      break;
              }
          }

          count++;
          
          // print the tuple
          switch (attr) {
              case 1:  // SELECT key
                  fprintf(stdout, "%d\n", key);
                  break;
              case 2:  // SELECT value
                  fprintf(stdout, "%s\n", value.c_str());
                  break;
              case 3:  // SELECT *
                  fprintf(stdout, "%d '%s'\n", key, value.c_str());
                  break;
          }
          next_tuple2:
          continue;
      }
      
  }
    

  end_if_unreasonable:
  // print matching tuple count if "select count(*)"
    
  fprintf(stdout,"\n");
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
    
  rc = 0;

    if (useindex){
        tree.close();
    }
  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
    RecordFile newrecord;//for new record
    RC rc;//for return value
    RecordId newrid;//store the location of the newrecord
    BTreeIndex tree;
    
    string line;//for each line
    int key;
    string value;
    
    ifstream infile;
    infile.open(loadfile.c_str());
    
    if(!infile.is_open()){
        fprintf(stderr,"The file %s cannot be opened.\n", loadfile.c_str());
        return RC_FILE_OPEN_FAILED;
    }
    
    rc = newrecord.open(table + ".tbl",'w');
    
    if (index){
        tree.open(table + ".idx", 'w');
        
        while (getline(infile,line)){
            parseLoadLine(line,key,value);
            rc = newrecord.append(key,value,newrid);
            if (rc != 0){
                return RC_FILE_WRITE_FAILED;
            }
            rc = tree.insert(key, newrid);
            if (rc != 0){
                return RC_FILE_WRITE_FAILED;
            }
        }
        tree.close();
    }
    
    else{
        while (getline(infile,line)){
            parseLoadLine(line,key,value);
            rc = newrecord.append(key,value,newrid);
        }
    }
    
    rc = newrecord.close();
    infile.close();

  return rc;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
