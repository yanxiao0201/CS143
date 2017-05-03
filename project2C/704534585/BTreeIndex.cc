/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
    fill(buffer, buffer + PageFile::PAGE_SIZE, 0);
    //recHeight = 0;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
    RC rc = pf.open(indexname, mode);
    if (rc != 0){
        return rc;
    }
    
    rc = pf.read(0, buffer);
    if (rc != 0){
        return rc;
    }
    
    PageId root;
    int height;
    
    memcpy(&root, buffer, sizeof(PageId));
    memcpy(&height, buffer, sizeof(int));
    
    if (root > 0){
        rootPid = root;
    }
    
    if (height > 0){
        treeHeight = height;
    }
    
    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    RC rc;
    if (rootPid > 0){
        memcpy(buffer, &rootPid, sizeof(PageId));
    }
    if (treeHeight > 0){
        memcpy(buffer + sizeof(PageId), &treeHeight, sizeof(int));
    }
    
    rc = pf.write(0, buffer);
    if (rc != 0){
        return rc;
    }
    
    rc = pf.close();
    
    return rc;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    RC rc;
    
    if (treeHeight == 0){
        
        BTLeafNode RootNode;
        RootNode.insert(key, rid);
        rootPid = pf.endPid();
        if (rootPid <= 0){
            rootPid = 1;
        }
        rc = RootNode.write(rootPid,pf);
        
        treeHeight = 1;
        return rc;
    }
    
 
    int recHeight = 1;
    int nodepid = rootPid;
    int midKey = -1;
    int midPid = -1;
    
    
    rc = insertrec(key, rid, recHeight, nodepid, midKey,midPid);
    
    return rc;
}

RC BTreeIndex::insertrec(int key, const RecordId& rid, int recHeight, int nodepid, int& midKey, int& midPid){
    RC rc;
    if (recHeight == treeHeight){//reach the leafnode
        BTLeafNode tempNode;
        tempNode.read(nodepid, pf);
        rc = tempNode.insert(key, rid);
        if (rc == 0){
            tempNode.write(nodepid,pf);
            return 0;
        }
        else{
            
            BTLeafNode tempNode2;

            rc = tempNode.insertAndSplit(key, rid, tempNode2, midKey);
            
            
            if (rc != 0){
                return rc;
            }
        
            int lastpid = pf.endPid();
            int prevpid = tempNode.getNextNodePtr();
            
            midPid = lastpid;
            
            tempNode2.setNextNodePtr(prevpid);
            tempNode.setNextNodePtr(lastpid);
            
            
            tempNode2.write(lastpid,pf);
            tempNode.write(nodepid,pf);
            
            if (treeHeight == 1){
                BTNonLeafNode RootNode;
                RootNode.initializeRoot(nodepid, midKey, lastpid);
                rootPid = pf.endPid();
                treeHeight++;
                
                RootNode.write(rootPid, pf);
               

            }
            return 0;
        }
    }
    
    else{
        
        BTNonLeafNode nonleafNode;
        nonleafNode.read(nodepid, pf);
        
        PageId child;
        child = nonleafNode.locateChildPtr(key, child);
        
        rc = insertrec(key, rid, recHeight+1, child, midKey, midPid);
        
        if (midKey >=0 && midPid >=0){
            rc = nonleafNode.insert(midKey,midPid);
            if (rc == 0){
                midKey = -1;
                midPid = -1;
                
                nonleafNode.write(nodepid, pf);
                return 0;
            }
            else {
                BTNonLeafNode nonleafNode2;
                nonleafNode.insertAndSplit(midKey, midPid, nonleafNode2, midKey);
                
                int lastpid = pf.endPid();
                
                midPid = lastpid;
                
                nonleafNode2.write(lastpid,pf);
                nonleafNode.write(nodepid,pf);
                
                if (recHeight == 1){
                    BTNonLeafNode RootNode;
                    RootNode.initializeRoot(nodepid, midKey, lastpid);
                    rootPid = pf.endPid();
                    treeHeight++;
                    
                    RootNode.write(rootPid, pf);
                    return 0;
                }
                return 0;
            
            }
        }
    }
    return 0;
}
/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    RC rc;
    int recHeight = 1;
    BTNonLeafNode nonLeaf;
    BTLeafNode leaf;
    PageId recPid = rootPid;
    int eid;
    
    while (recHeight < treeHeight){
        rc = nonLeaf.read(recPid, pf);
        if (rc != 0){
            return rc;
        }
        rc = nonLeaf.locateChildPtr(searchKey, recPid);
        if (rc != 0){
            return rc;
        }
        recHeight++;
    }
    rc = leaf.read(recPid, pf);

    
    rc = leaf.locate(searchKey, eid);

    cursor.eid = eid;
    cursor.pid = recPid;
    
    if (rc != 0){
        return rc;
    }
    
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    RC rc;
    BTLeafNode leaf;
    rc = leaf.read(cursor.pid, pf);
    if (rc != 0){
        return rc;
    }
    
    rc = leaf.readEntry(cursor.eid, key, rid);
    if (rc != 0){
        return rc;
    }
    
    if (cursor.eid + 1 > leaf.getKeyCount()){
        cursor.eid = 0;
        cursor.pid = leaf.getNextNodePtr();
    }
    
    cursor.eid ++;

    return 0;
}
