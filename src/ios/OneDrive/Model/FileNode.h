//
//  FileNode.h
//  OneDrive
//
//  Created by Ruchin Kansal on 09/04/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#ifndef OneDrive_FileNode_h
#define OneDrive_FileNode_h
#include "ODDataController.h"

#include "CommonUtils.h"
#include <set>

class FileNode: public enable_shared_from_this<FileNode>
{
public:
    
    typedef NewGroup::Core::Utils::TypeFactory<FileNode>::SharedPtr SharedPtr;
    typedef NewGroup::Core::Utils::TypeFactory<FileNode>::WeakPtr WeakPtr;
    
    struct ChildComparator
    {
        bool operator()(FileNode::WeakPtr a, FileNode::WeakPtr b)
        {
            auto x = a.lock();
            auto y = b.lock();
            
            if(x && y)
            {
                return *x < *y;
            }
            
            return false;
        }
    };
    
    typedef std::vector<WeakPtr> ChildrenContainer;
                
    static void UpdatFileNodeWithCustomChildren(FileNode::SharedPtr rootNode,std::vector<FileNode::SharedPtr> childList);
                
    static void AddFileNodes(std::vector<FileNode::SharedPtr> &childList, const ODMediaList& m) ;
                
                FileNode(){}
    FileNode (const ODMedia &m, FileNode::SharedPtr parent);
                
                void UpdateMedia(const ODMedia &m);
    const ChildrenContainer& getChildren() const;
    ChildrenContainer& getChildren();
    bool operator < (const FileNode&) const;
    
    void ChangeChildrenOrder(eSiblingOrderPolicy order, bool ascending = true, bool forcedReorder = false);
    
    bool isMediaValid() const
    {
        return mMediaValid;
    }
    
    const ODMedia &GetMedia() const
                {
                    return mMedia;
                }
    FileNode::SharedPtr GetChild(int index) const
                {
                    if(index >= mChildren.size())
                        return nullptr;
                    
                    auto childIt = std::next(mChildren.begin(), index);
                    return childIt->lock();
                }
                const UString& GetName() const
                {
                    return mName;
                }
       FileNode::SharedPtr GetParent() const
                {
                    return mParent;
                }
                
                void ReorderChildren(bool ascending = true);
                
                const UString& GetLowerCaseName() const{
                    return mLowerName;
                }
                
                void UpdateParent(FileNode::SharedPtr parent);
                void RemoveChild(FileNode::SharedPtr child);
                
private:
                UString mName, mKey, mLowerName;
    FileNode::SharedPtr mParent;
    ChildrenContainer mChildren;
    eSiblingOrderPolicy mCurrentOrderPolicy;
    bool mLastOrderValid;
    bool mMediaValid;
    ODMedia mMedia;
};


#endif
