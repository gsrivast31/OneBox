//
//  TreeViewCell.h
//  OneDrive
//
//  Created by Vikash Jindal on 18/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TreeViewCell : UITableViewCell
@property (nonatomic, strong) UILabel *valueLabel;
@property (nonatomic, strong) UIImageView *arrowImage;
@property (nonatomic) int level;
@property (nonatomic) BOOL expanded;

- (id)initWithStyle:(UITableViewCellStyle)style
	reuseIdentifier:(NSString *)reuseIdentifier
			  level:(NSUInteger)level
		   expanded:(BOOL)expanded;
@end
