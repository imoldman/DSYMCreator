//
//  ViewController.m
//  TestApp
//
//  Created by oldman on 9/2/16.
//  Copyright © 2016 oldman. All rights reserved.
//

#import "ViewController.h"

@interface ViewController () <UITextFieldDelegate>
@property (nonatomic, weak) IBOutlet UITextField* passwordTextField;
@end

@implementation ViewController

- (IBAction)verifyClick:(id)sender
{
    NSString* text = self.passwordTextField.text;
    [self foo:text];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return NO;
}

- (void)foo:(NSString*)text
{
    NSString* lastStr = [text substringWithRange:NSMakeRange(text.length - 1, 1)];
    [self foo1:[text componentsSeparatedByString:@"-"] :lastStr];
}

- (void)foo1:(NSArray*)array :(NSString*)text
{
    [self foo2:array[0] :text];
}

- (void)foo2:(NSString*)text : (NSString*)text2
{
    [self foo3:[text stringByAppendingString:text2]];
}

- (void)foo3:(NSString*)text
{
    if (![text isEqualToString:@"22"]) {
        UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"Error" message:@"Verify Failed" preferredStyle:UIAlertControllerStyleAlert];
        [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
        [self presentViewController:alert animated:YES completion:nil];
    }
}


@end
