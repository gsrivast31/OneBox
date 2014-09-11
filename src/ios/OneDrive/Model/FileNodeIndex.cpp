//
//  FileNodeIndex.cpp
//  OneDrive
//
//  Created by Ruchin Kansal on 09/04/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#include "FileNodeIndex.h"
#include "assert.h"

std::map<long, FileNodeIndex::SharedPtr> FileNodeIndex::mAccountMap;

FileNodeIndex::SharedPtr FileNodeIndex::GetFileNodeIndex(int accountID)
{
    return mAccountMap[accountID];
}

void FileNodeIndex::AddFileNodeIndex(int accountID)
{
    mAccountMap[accountID] = std::make_shared<FileNodeIndex>();
}

FileNode::SharedPtr FileNodeIndex::GetRootNode()
{
    FileNodeLock lock(mMutex);
    ID rootKey = -1;
    auto rootNode = GetNode(rootKey);
    if(!rootNode)
    {
        rootNode.reset(new FileNode(ODMedia(), FileNode::SharedPtr()));
        mNodeIndex[rootKey] = rootNode;
    }
    
    return rootNode;
}

FileNode::SharedPtr FileNodeIndex::GetNode(ID mediaID)
{
    FileNodeLock lock(mMutex);
    //auto key = NormalizeKey(inKey);
    auto fileNode = mNodeIndex.find(mediaID);
    if(fileNode == mNodeIndex.end())
        return FileNode::SharedPtr();
    else
        return fileNode->second;
    
}

FileNode::SharedPtr FileNodeIndex::AddNode(const ODMedia& media)
{
    FileNodeLock lock(mMutex);
    //auto key = NormalizeKey(inKey);
    
    FileNode::SharedPtr parentNode = media.parent_id == -1 ?  GetRootNode() : GetNode(media.parent_id);
    FileNode::SharedPtr addedNode = GetNode(media.media_id);
    
    assert(parentNode);
    
    if(!addedNode)
    {
        addedNode.reset(new FileNode());
        mNodeIndex[media.media_id] = addedNode;
    }
    
    addedNode->UpdateMedia(media);
    
    if(addedNode->GetParent() != parentNode)
    {
        //Change the parent
        addedNode->UpdateParent(parentNode);
    }
    
    return addedNode;
}

void FileNodeIndex::RemoveNode(ID mediaID, bool recursive)
{
    FileNodeLock lock(mMutex);
    //auto key = NormalizeKey(inKey);
    if(recursive)
    {
        auto fileNode = GetNode(mediaID);
        if(fileNode)
        {
            auto children = fileNode->getChildren();
            for(auto child:children)
            {
                auto childPtr = child.lock();
                
                if(childPtr)
                {
                    auto media = childPtr->GetMedia();
                    RemoveNode(media.media_id, recursive);
                }
                    
            }
        }
    }
    
    {
        auto fileNode = GetNode(mediaID);
        if (fileNode) {
            auto parentNode = fileNode->GetParent();
            parentNode->RemoveChild(fileNode);
            mNodeIndex.erase(mediaID);
        }
    }
}