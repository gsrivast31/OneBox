//
//  TreeNode.h
//  OneDrive
//
//  Created by Vikash Jindal on 18/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TreeNode : NSObject
{
    NSArray *flattenedTreeCache;
    NSArray *ascendantTreeCache;
    //id attribute;
}
//@property (nonatomic) int index;
@property (nonatomic, strong) NSString *value;
@property (nonatomic, strong) TreeNode *parent;
@property (nonatomic, strong) NSMutableDictionary *children;
@property (nonatomic) BOOL inclusive;
@property (nonatomic, strong) id attribute;;

- (id)initWithValue:(NSString *)value;

- (void)addChild:(TreeNode *)newChild forIdentifier:(NSString *) string;
- (NSUInteger)descendantCount;
- (NSArray *)ascendants;
- (NSUInteger)levelDepth;
- (NSArray *)flattenElements;
- (NSArray *)flattenElementsWithCacheRefresh:(BOOL)invalidate;
- (BOOL)isRoot;
- (BOOL)hasChildren;
-(id)getNodeMedia;
@end
