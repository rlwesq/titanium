/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#import "growl_delegate.h"
#include <kroll/kroll.h>

@implementation TiGrowlDelegate

-(id) init
{
	self = [super init];
	
	if (self != nil)
	{
		[GrowlApplicationBridge setGrowlDelegate:self];
	}
	
	return self;
}

-(BOOL) growlReady {
	return ready;
}

//// GrowlDelegate callbacks

-(void) growlIsReady
{
	NSLog(@">>>>>>>>>>> Growl is ready");
	ready = TRUE;
}

- (void) growlNotificationWasClicked:(id)clickContext
{
	NSLog(@">>>>>>>>>>>>>>> Growl notification clicked");
	NSMutableArray* array = (NSMutableArray*) clickContext;
	MethodWrapper* wrapper = (MethodWrapper*) [array objectAtIndex:0];
	SharedKMethod* method = [wrapper method];
	if (!method->isNull())
	{
		ValueList args;
		(*method)->Call(args);
	}
	[array release]; // after callback release the reference
}

@end

@implementation MethodWrapper

-(id) initWithMethod:(SharedBoundMethod*)m
{
	self = [super init];
	
	if (self != nil) {
		method = m;
	}
	
	return self;
}

- (SharedBoundMethod *) method
{
	return method;
}

- (void) dealloc
{
	delete method;
	[super dealloc];
}

@end
