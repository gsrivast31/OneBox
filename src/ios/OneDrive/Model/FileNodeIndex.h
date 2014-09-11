//
//  FileNodeIndex.h
//  OneDrive
//
//  Created by Ruchin Kansal on 09/04/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#ifndef OneDrive_FileNodeIndex_h
#define OneDrive_FileNodeIndex_h
#include "FileNode.h"
#include <map>
#include "UString.h"
#include <mutex>

class FileNodeIndex
{
    
    
    std::map<ID, FileNode::SharedPtr> mNodeIndex;
public:
    typedef NewGroup::Core::Utils::TypeFactory<FileNodeIndex>::SharedPtr SharedPtr;
    
    static SharedPtr GetFileNodeIndex(int accountID);
    static void AddFileNodeIndex(int accountID);
    FileNode::SharedPtr GetRootNode();
    FileNode::SharedPtr GetNode(ID mediaID);
    FileNode::SharedPtr AddNode(const ODMedia& media);
    void RemoveNode(ID mediaID, bool recursive = true);
    
    private:
    static std::map<long, SharedPtr> mAccountMap;
    std::recursive_mutex mMutex;
    typedef std::unique_lock<std::recursive_mutex> FileNodeLock;
    
};


#endif
