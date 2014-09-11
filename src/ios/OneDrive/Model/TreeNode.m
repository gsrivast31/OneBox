//
//  TreeNode.m
//  OneDrive
//
//  Created by Vikash Jindal on 18/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "TreeNode.h"
#import "Media.h"
@implementation TreeNode

- (id)initWithValue:(NSString *)value {
	self = [super init];
	if (self) {
		_value = value;
		_inclusive = YES;
        _children = [[NSMutableDictionary alloc] initWithCapacity:1];
	}
	
	return self;
}

- (NSUInteger)descendantCount {
	NSUInteger cnt = 0;
	
	for (NSString* key in self.children) {
		if (self.inclusive) {
			cnt++;
            TreeNode *child = [self.children objectForKey:key];
			if (child.children.count > 0) {
				cnt += [child descendantCount];
			}
		}
	}
	
	return cnt;
}

- (NSArray *)ascendants {
    if (!ascendantTreeCache) {
        NSMutableArray* upTree = [[NSMutableArray alloc] init];
        TreeNode* parentNode = self.parent;
        while (parentNode) {
            [upTree addObject:parentNode];
            parentNode = parentNode.parent;
        }
        
        ascendantTreeCache = [[NSArray alloc] initWithArray:upTree];
    }
    
    return ascendantTreeCache;
}

- (NSArray *)flattenElements {
	return [self flattenElementsWithCacheRefresh:NO];
}

- (NSArray *)flattenElementsWithCacheRefresh:(BOOL)invalidate {
	if (!flattenedTreeCache || invalidate) {
		//if there was a previous cache and due for invalidate, release resources first
		if (flattenedTreeCache) {
			flattenedTreeCache = nil;
		}
    
		NSMutableArray *allElements = [[NSMutableArray alloc] initWithCapacity:[self descendantCount]];
		[allElements addObject:self];
		
		if (_inclusive) {
			for (NSString* key in self.children) {
                 TreeNode *child = [self.children objectForKey:key];
				[allElements addObjectsFromArray:[child flattenElementsWithCacheRefresh:invalidate]];
			}
		}
		
		flattenedTreeCache = [[NSArray alloc] initWithArray:allElements];
	}
	
	return flattenedTreeCache;
}

- (void)addChild:(TreeNode *)newChild forIdentifier:(NSString *) string
{
    newChild.parent = self;
    [self.children setObject:newChild forKey:string];
}
//- (void)addChild:(TreeNode *)newChild {
//	newChild.parent = self;
//	[self.children addObject:newChild];
//}

- (NSUInteger)levelDepth {
	if (!_parent) return 0;
	
	NSUInteger cnt = 0;
	cnt++;
	cnt += [_parent levelDepth];
	
	return cnt;
}

- (BOOL)isRoot {
	return (!_parent);
}

- (BOOL)hasChildren {
	return (self.children.count > 0);
}

-(id)getNodeMedia{
    Media *media = nil;
    if([self.attribute isKindOfClass:[Media class]])
    {
         media = self.attribute;
    }
    
    return media;
}
@end
