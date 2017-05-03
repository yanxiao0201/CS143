#include "BTreeNode.h"
#include <cstring>
#include <iostream>
#include <cstdlib>

/* BTreeNode structure: 4 bytes for int key, then 8 bytes for RecordId
 */

using namespace std;


//constructor
BTLeafNode::BTLeafNode(){
    numKeys = 0;
    fill(buffer, buffer + PageFile::PAGE_SIZE, 0);
}
/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{
    RC rc;
    rc = pf.read(pid, buffer);
    return rc;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{
    RC rc;
    rc = pf.write(pid, buffer);
    return rc;

}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{
    int length = sizeof(RecordId) + sizeof(int);
    int count = 0;
    char * pointer = buffer;
    
    int key;
    

    int limit = PageFile::PAGE_SIZE - sizeof(PageId);//last thing in node must be PageId
    int maxnum = limit/length;
    
    int i;
    
    for (i = 0; i < maxnum; i++){
        memcpy(&key, pointer, sizeof(int));
        if (key == 0){
            break;
        }
        else {
            count++;
            pointer += length;
        }
    }
    
    return count;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{
    int length = sizeof(RecordId) + sizeof(int);
    int keycount = getKeyCount();
    
    int limit = PageFile::PAGE_SIZE - sizeof(PageId);//last thing in node must be PageId
    int maxnum = limit/length;

    if (keycount >= maxnum){
        return RC_NODE_FULL;
    }
    
    char newbuffer[PageFile::PAGE_SIZE];
    fill(newbuffer, newbuffer + PageFile::PAGE_SIZE, 0);
    
    char * newbuf = newbuffer;
    char * oldbuf = buffer;
    
    int i;
    int oldkey;
    
    for (i = 0;  i < keycount * length; i+= length){
        memcpy(&oldkey, oldbuf, sizeof(int));
        

        if (oldkey > key){
            break;
        }
        else {
            oldbuf += length;
        }
    }
    // i is the place to insert the key
    memcpy(newbuf, buffer, i);
    
    memcpy(newbuf + i, &key, sizeof(int));
    memcpy(newbuf + i + sizeof(int), &rid, sizeof(RecordId));
    
    memcpy(newbuf + i + length, buffer + i, keycount * length - i);
    
    PageId pid = getNextNodePtr(); //add pageid to the end
    
    memcpy(newbuf + PageFile::PAGE_SIZE - sizeof(PageId), &pid, sizeof(PageId));
    memcpy(buffer, newbuffer, PageFile::PAGE_SIZE);
    
    
    return 0;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{
    int length = sizeof(RecordId) + sizeof(int);
    int keycount = getKeyCount();
    
    int limit = PageFile::PAGE_SIZE - sizeof(PageId);//last thing in node must be PageId
    int maxnum = limit/length;
    
    if (keycount < maxnum){
        return RC_INVALID_FILE_MODE;
    }
    if (sibling.getKeyCount()!= 0){
        return RC_INVALID_FILE_FORMAT;
    }
    
    fill(sibling.buffer,sibling.buffer + PageFile::PAGE_SIZE, 0);
    
    int halfkeynum = (keycount + 1)/2;
    
    int halflength = halfkeynum * length;
    
    //cpy the last half of the buff to the sibling
    memcpy(sibling.buffer, buffer + halflength, PageFile::PAGE_SIZE - sizeof(PageId) - halflength);
    memset(buffer + halflength, 0, PageFile::PAGE_SIZE - sizeof(PageId) - halflength);
    sibling.setNextNodePtr(getNextNodePtr());
    
    //deciding which half to insert the new key and rid
    int firstkey;
    memcpy(&firstkey, sibling.buffer, sizeof(int));
    
    if (key >= firstkey){
        sibling.insert (key, rid);
        
    }
    else{
        insert(key, rid);
    }
    
    memcpy(&siblingKey, sibling.buffer, sizeof(int));
    
    return 0;
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{
    int length = sizeof(RecordId) + sizeof(int);
    int keycount = getKeyCount();

    int i;
    int key;
    char * bufpointer = buffer;
    for (i = 0; i < keycount * length; i+=length){
        memcpy(&key, bufpointer, sizeof(int));
        if (searchKey == key){
            eid = i/length;
            return 0;
        }
        else if (searchKey > key){
            bufpointer += length;
        }
        else{
            eid = i/length;
            return RC_NO_SUCH_RECORD;
        }
    }
    
    eid = keycount;
    return RC_NO_SUCH_RECORD;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
    if (eid < 0 || eid > getKeyCount()){
        return RC_NO_SUCH_RECORD;
    }
    
    int length = sizeof(RecordId) + sizeof(int);
    char * bufpointer = buffer;
    memcpy(&key, bufpointer + eid * length, sizeof(int));
    memcpy(&rid, bufpointer + eid * length + sizeof(int), sizeof(RecordId));
    
    
    return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{
    PageId pid;
    char * bufpointer = buffer;
    memcpy(&pid, bufpointer + PageFile::PAGE_SIZE - sizeof(PageId), sizeof(PageId));
    return pid;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{
    if (pid < 0){
        
        return RC_INVALID_PID;
    }
    char * bufpointer = buffer;
    memcpy(bufpointer + PageFile::PAGE_SIZE - sizeof(PageId), &pid, sizeof(PageId));
    
    return 0;
}

//constructor
BTNonLeafNode::BTNonLeafNode(){
    numKeys = 0;
    fill(buffer, buffer + PageFile::PAGE_SIZE, 0);
}
/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{
    RC rc;
    rc = pf.read(pid, buffer);
    return rc;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{
    RC rc;
    rc = pf.write(pid, buffer);
    return rc;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{
    int length = sizeof(PageId) + sizeof(int);
    int count = 0;
    char * pointer = buffer + length;

    int key = -1;
    
    
    int limit = PageFile::PAGE_SIZE;
    int maxnum = limit/length - 1; //first key being empty
    
    int i;
    
    for (i = 1; i <= maxnum; i++){
        memcpy(&key, pointer, sizeof(int));
        if (key == 0){
            break;
        }
        else {
            count++;
            pointer += length;
        }
    }
    
    return count;
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{
    int length = sizeof(PageId) + sizeof(int);
    int keycount = getKeyCount();
    
    int limit = PageFile::PAGE_SIZE;
    int maxnum = limit/length -1; //first key being empty
    
    if (keycount == maxnum){
        return RC_NODE_FULL;
    }
    
    char newbuffer[PageFile::PAGE_SIZE];
    fill(newbuffer, newbuffer + PageFile::PAGE_SIZE, 0);
    
    char * newbuf = newbuffer;
    char * oldbuf = buffer + length;
    
    int i;
    int oldkey;
    
    for (i = length;  i < limit; i+= length){
        memcpy(&oldkey, oldbuf, sizeof(int));
        if (oldkey == 0){
            break;
        }
        else if (oldkey > key){
            break;
        }
        else {
            oldbuf += length;
        }
    }
    // i is the place to insert the key
    memcpy(newbuf, buffer, i);
    
    memcpy(newbuf + i, &key, sizeof(int));
    memcpy(newbuf + i + sizeof(int), &pid, sizeof(PageId));
    
    memcpy(newbuf + i + length, buffer + i, (keycount + 1) * length - i);
    

    memcpy(buffer, newbuffer, PageFile::PAGE_SIZE);

    return 0;
    
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{
    int length = sizeof(PageId) + sizeof(int);
    int keycount = getKeyCount();
    
    int limit = PageFile::PAGE_SIZE;
    int maxnum = limit/length - 1; //first must be empty
    
    if (keycount < maxnum){
        return RC_INVALID_FILE_MODE;
    }
    if (sibling.getKeyCount()!= 0){
        return RC_INVALID_FILE_FORMAT;
    }
    fill(sibling.buffer, sibling.buffer + PageFile::PAGE_SIZE, 0);
    
    int halfkeynum = (keycount + 1)/2;
    
    int halflength = halfkeynum * length + length;
    
    //get the last one for the first half and the first one for the last half
    int firsthalfkey = -1;
    int lasthalfkey = -1;
    
    memcpy(&firsthalfkey, buffer + halflength - length, sizeof(int));
    memcpy(&lasthalfkey, buffer + halflength, sizeof(int));
    
    if (key < firsthalfkey){//firsthalfkey need to be extracted and returned
        memcpy(sibling.buffer + length, buffer + halflength, PageFile::PAGE_SIZE - halflength);
        
        memcpy(sibling.buffer + sizeof(int), buffer + halflength - sizeof(PageId),sizeof(PageId));
        
        
        memset(buffer + halflength, 0, PageFile::PAGE_SIZE - halflength);
        
        memset(buffer + halflength - length, 0, length);
        
        insert(key, pid);
        
        midKey = firsthalfkey;
    }
    else if (key > lasthalfkey){//lasthalfkey need to be extraced and returned
        memcpy(sibling.buffer, buffer + halflength, PageFile::PAGE_SIZE - halflength);
        memset(sibling.buffer, 0, sizeof(int));
        memset(buffer + halflength, 0, PageFile::PAGE_SIZE - halflength);
        sibling.insert(key, pid);
        midKey = lasthalfkey;
    }
    else {//key needs to be extracted
        memcpy(sibling.buffer + length, buffer + halflength, PageFile::PAGE_SIZE - halflength);
        memcpy(sibling.buffer + sizeof(int), &pid, sizeof(PageId));
        memset(buffer + halflength, 0, PageFile::PAGE_SIZE - halflength);
        midKey = key;
    }
    

    return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
    int length = sizeof(PageId) + sizeof(int);
    int keycount = getKeyCount();
    
    
    int i;
    int key;
    
    pid = 0;
    
    char * bufpointer = buffer + 8;
    for (i = 8; i < keycount * length + length; i+=length){
        memcpy(&key, bufpointer, sizeof(int));
        
        if (i == 8 && key > searchKey){
            memcpy(&pid, buffer + 4, sizeof(PageId));
            return 0;
        }
        else if (key > searchKey){
            memcpy(&pid, bufpointer -4, sizeof(PageId));
            return 0;
        }
        else{
            bufpointer += length;
            
        }
    }
    
    memcpy(&pid, bufpointer - sizeof(PageId), sizeof(PageId));
    return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{
    fill(buffer, buffer + PageFile::PAGE_SIZE, 0);

    memcpy(buffer + sizeof(int), &pid1, sizeof(PageId));
    memcpy(buffer + 8, &key, sizeof(int));
    memcpy(buffer + 12, &pid2, sizeof(PageId));
    
    return 0;
}














