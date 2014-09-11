//
//  TreeViewCell.m
//  OneDrive
//
//  Created by Vikash Jindal on 18/12/12.
//  Copyright (c) 2012 Vikash Jindal. All rights reserved.
//

#import "TreeViewCell.h"
#define IMG_HEIGHT_WIDTH 20
#define CELL_HEIGHT 44
#define SCREEN_WIDTH 320
#define LEVEL_INDENT 32
#define YOFFSET 12
#define XOFFSET 6

@interface TreeViewCell (Private)

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor
						selectedColor:(UIColor *)selectedColor
							 fontSize:(CGFloat)fontSize
								 bold:(BOOL)bold;

@end

@implementation TreeViewCell

- (id)initWithStyle:(UITableViewCellStyle)style
	reuseIdentifier:(NSString *)reuseIdentifier
			  level:(NSUInteger)level
		   expanded:(BOOL)expanded {
    
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
		self.level = level;
		self.expanded = expanded;
		
		UIView *content = self.contentView;
		
		self.valueLabel =
        [self newLabelWithPrimaryColor:[UIColor blackColor]
                         selectedColor:[UIColor whiteColor]
                              fontSize:20.0 bold:YES];
		self.valueLabel.textAlignment = NSTextAlignmentLeft;
		[content addSubview:self.valueLabel];
		
		self.arrowImage =
        [[UIImageView alloc] initWithImage:
         [UIImage imageNamed:self.expanded ?
          @"CircleArrowDown_sml" : @"CircleArrowRight_sml"]];
		[content addSubview:self.arrowImage];
    }
    return self;
}



#pragma mark -
#pragma mark Other overrides

- (void)layoutSubviews {
    [super layoutSubviews];
    CGRect contentRect = self.contentView.bounds;
	
    if (!self.editing) {
		
		// get the X pixel spot
        CGFloat boundsX = contentRect.origin.x;
		CGRect frame;
		
		frame = CGRectMake((boundsX + self.level + 1) * LEVEL_INDENT,
						   0,
						   self.bounds.size.width - (self.level * LEVEL_INDENT),
						   CELL_HEIGHT);
		self.valueLabel.frame = frame;
		
		CGRect imgFrame;
		imgFrame = CGRectMake(((boundsX + self.level + 1) * LEVEL_INDENT) - (IMG_HEIGHT_WIDTH + XOFFSET),
							  YOFFSET,
							  IMG_HEIGHT_WIDTH,
							  IMG_HEIGHT_WIDTH);
		self.arrowImage.frame = imgFrame;
	}
}

#pragma mark -
#pragma mark Private category

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor
						selectedColor:(UIColor *)selectedColor
							 fontSize:(CGFloat)fontSize
								 bold:(BOOL)bold {
    UIFont *font;
    if (bold) {
        font = [UIFont boldSystemFontOfSize:fontSize];
    } else {
        font = [UIFont systemFontOfSize:fontSize];
    }
	
	UILabel *newLabel = [[UILabel alloc] initWithFrame:CGRectZero];
	newLabel.backgroundColor = [UIColor whiteColor];
	newLabel.opaque = YES;
	newLabel.textColor = primaryColor;
	newLabel.highlightedTextColor = selectedColor;
	newLabel.font = font;
	newLabel.numberOfLines = 0;
	
	return newLabel;
}

@end