//
//  FileNode.cpp
//  OneDrive
//
//  Created by Ruchin Kansal on 09/04/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#include "FileNode.h"

const UString kDummyNode("dummyNode");

FileNode::FileNode (const ODMedia &m, FileNode::SharedPtr parent)
{
    UpdateMedia(m);
    mParent = parent;
}

void FileNode::UpdateMedia(const ODMedia &m)
{
    //Extract name from media full path
    if(m.full_path.size())
    {
        std::vector<UString> stringVector;
        std::wstring del = L"/";
        m.full_path.Split(stringVector,del.c_str());
        mName = stringVector[stringVector.size()-1];
        
    }
    
    mMediaValid = true;
    mMedia = m;
    mLowerName = UString::fromPlatform([mName.getPlatformString() lowercaseString]);
}

const FileNode::ChildrenContainer& FileNode::getChildren() const
{   
    return mChildren;
}

FileNode::ChildrenContainer& FileNode::getChildren()
{
        
    return mChildren;
}

void FileNode::UpdatFileNodeWithCustomChildren(FileNode::SharedPtr rootNode,std::vector<FileNode::SharedPtr> childList)
{
   
    rootNode->getChildren().clear();
    for(auto child:childList)
    {
        child->mParent = rootNode;
        rootNode->getChildren().push_back(child);
    }
}

void FileNode::AddFileNodes(std::vector<FileNode::SharedPtr> &childList, const ODMediaList& m)
{
    childList.clear();
    for(int i = 0 ; i < m.size(); i++)
    {
        const ODMedia md = m[i];
        childList.push_back(std::shared_ptr<FileNode>(new  FileNode(md,NULL)));
    }

}

bool FileNode::operator < (const FileNode& other) const
{
    if(!isMediaValid() || !other.isMediaValid())
    {
        return this < &other;
    }
    
    auto otherMedia = other.GetMedia();
    switch(mCurrentOrderPolicy)
    {
        case kOrderByDate:
        {
            if(mMedia.mod_date.GetUTCTime() == other.GetMedia().mod_date.GetUTCTime())
                return GetLowerCaseName() < other.GetLowerCaseName();
            else
                return mMedia.mod_date.GetUTCTime() < other.GetMedia().mod_date.GetUTCTime();
        }
            break;
        case kOrderByName:
        {
            return GetLowerCaseName() < other.GetLowerCaseName();
        }
            break;
        case kOrderBySize:
        {
            if(mMedia.size == other.GetMedia().size)
                return GetLowerCaseName() < other.GetLowerCaseName();
            else
                return mMedia.size < other.GetMedia().size;
        }
            break;
        case kOrderByType:
        {
            if(mMedia.mime_type == other.GetMedia().mime_type)
                return GetLowerCaseName() < other.GetLowerCaseName();
            else
                return mMedia.mime_type < other.GetMedia().mime_type;
            
        }
            break;
    }
    
    return this < &other;
}

void FileNode::ChangeChildrenOrder(eSiblingOrderPolicy order, bool ascending, bool forcedReorder)
{
    if(mCurrentOrderPolicy == order && !forcedReorder)
        return;
    
    mCurrentOrderPolicy = order;
    mLastOrderValid = false;

    for(auto child:mChildren)
    {
        auto childPtr = child.lock();
        if(childPtr)
        {
            childPtr->ChangeChildrenOrder(order, ascending, forcedReorder);
            //tempChildren.insert(childPtr);
        }
    }
    
    ReorderChildren(ascending);
    
    mLastOrderValid = true;
}

void FileNode::ReorderChildren(bool ascending)
{
    ChildComparator comparator;
    std::sort(mChildren.begin(), mChildren.end(), comparator);

    if (!ascending) {
        std::reverse(mChildren.begin(), mChildren.end());
    }
}

void FileNode::UpdateParent(FileNode::SharedPtr parent)
{
    if(mParent != parent)
    {
        if(mParent)
            mParent->RemoveChild(shared_from_this());
        mParent = parent;
        mParent->getChildren().push_back(shared_from_this());
    }
}

void FileNode::RemoveChild(FileNode::SharedPtr child)
{
    auto iter = mChildren.begin();
    for (; iter != mChildren.end(); iter++) {
        if (iter->lock() == child) {
            break;
        }
    }
    
    if (iter != mChildren.end()) {
        mChildren.erase(iter);
    }
}