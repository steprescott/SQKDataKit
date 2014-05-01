//
//  SQKManagedObjectController.h
//  SQKManagedObjectController
//
//  Created by Sam Oakley on 20/03/2014.
//  Copyright (c) 2014 Sam Oakley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

@class SQKManagedObjectController;

extern NSString* const SQKManagedObjectControllerErrorDomain;

typedef void (^SQKManagedObjectControllerObjectsChangedBlock)(NSIndexSet *changedObjectIndexes);
typedef void (^SQKManagedObjectControllerObjectsFetchedBlock)(NSIndexSet *changedObjectIndexes, NSError *error);


/**
 *  The delegate of a SQKManagedObjectController object must adopt the SQKManagedObjectControllerDeledate protocol. 
 *  Optional methods of the protocol allow the delegate to be informed of changes to the underlying managed objects.
 */
@protocol SQKManagedObjectControllerDelegate <NSObject>
@optional

/**
 *  Called when objects are fetched as a result of a call to performFetch: or performFetchAsynchronously.
 *  Always called on the main thread.
 *
 *  @param controller           The SQKManagedObjectController where the objects are fetched.
 *  @param fetchedObjectIndexes The indexes of the newly fetched objects. Will be all objects in the array.
 *  @param error                If the fetch is not successful, this will be an error object that describes the problem.
 */
-(void)controller:(SQKManagedObjectController*)controller
   fetchedObjects:(NSIndexSet*)fetchedObjectIndexes error:(NSError**)error;

/**
 *  Called when objects are updated after the main context is saved or changes are merged from a background thread.
 *
 *  @param controller           The SQKManagedObjectController where the changes occured.
 *  @param changedObjectIndexes The indexes of the updated objects.
 */
-(void)controller:(SQKManagedObjectController*)controller
   updatedObjects:(NSIndexSet*)updatedObjectIndexes;

/**
 *  Called when objects are deleted after the main context is saved or changes are merged from a background thread.
 *
 *  @param controller           The SQKManagedObjectController where the deletions occured.
 *  @param changedObjectIndexes The indexes of the deleted objects.
 */
-(void)controller:(SQKManagedObjectController*)controller
   deletedObjects:(NSIndexSet*)deletedObjectIndexes;
@end

/**
 *  This class helps manage NSManagedObjects in a similar way to NSFetchedResultsController.
 *  Should be used from the main thread unless otherwise noted.
 */
@interface SQKManagedObjectController : NSObject
@property (nonatomic, strong, readonly) NSFetchRequest *fetchRequest;
@property (nonatomic, strong, readonly) NSArray *managedObjects;
@property (nonatomic, strong, readonly) NSManagedObjectContext *managedObjectContext;

@property (nonatomic, weak) id<SQKManagedObjectControllerDelegate> delegate;

/**
 *  A block callback to be called when objects are fetched as a result of performFetch:/performFetchAsync.
 */
@property (nonatomic, copy) SQKManagedObjectControllerObjectsFetchedBlock fetchedObjectsBlock;

/**
 *  A block callback to be called when objects are updated.
 */
@property (nonatomic, copy) SQKManagedObjectControllerObjectsChangedBlock updatedObjectsBlock;

/**
 *  A block callback to be called when objects are deleted.
 */
@property (nonatomic, copy) SQKManagedObjectControllerObjectsChangedBlock deletedObjectsBlock;

/**
 *  Returns a SQKManagedObjectController set up with the given fetch request and context.
 *  The fetch request is not executed until performFetch:/performFetchAsync is called.
 *
 *  @param fetchRequest A fetch request that specifies the search criteria for the fetch. Must not be nil.
 *  @param context      The managed object context to use. Must be created with NSMainQueueConcurrencyType, and must not be nil.
 *
 *  @return An initialised SQKManagedObjectController.
 */
- (instancetype)initWithFetchRequest:(NSFetchRequest *)fetchRequest managedObjectContext:(NSManagedObjectContext *)context;

/**
 *  Returns a SQKManagedObjectController that manages the given array of NSManagedObjects.
 *  Used when you want to monitor changes on already-fetched objects.
 *  performFetch:/performFetchAsync are non-op for this controller.
 *
 *  @param managedObjects An array of NSManagedObjects created on a context with NSMainQueueConcurrencyType. Must not be nil.
 *
 *  @return An initialised SQKManagedObjectController.
 */
- (instancetype)initWithWithManagedObjects:(NSArray *)managedObjects;

/**
 *  Returns a SQKManagedObjectController that manages the given NSManagedObject.
 *  Used when you want to monitor changes on an already-fetched object.
 *  performFetch:/performFetchAsync are non-op for this controller.
 *
 *  @param managedObjects A NSManagedObject created on a context with NSMainQueueConcurrencyType. Must not be nil.
 *
 *  @return An initialised SQKManagedObjectController.
 */
- (instancetype)initWithWithManagedObject:(NSManagedObject *)managedObject;

/**
 *  Execute the fetch request and store the results in self.managedObjects.
 *  Blocks the main thread. On returning, fetchedObjects will be available.
 *
 *  @param error If there is a problem executing the fetch, upon return contains an instance of NSError that describes the problem.
 *
 *  @return YES if the fetch succeeds, otherwise NO.
 */
- (BOOL)performFetch:(NSError**)error;

/**
 *  Execute the fetch request in a background thread and store the results in self.managedObjects.
 *  Provide a delegate or set fetchedObjectsBlock to perfom an action once the objects are fetched.
 *  You must call [managedObjectContext save:] to commit these changes.
 */
- (void)performFetchAsynchronously;

/**
 *  Deleted the fetched objects from self.managedObjectContext and saves.
 *  self.managedObjects must contain objects.
 *  You must call [managedObjectContext save:] to commit these changes.
 *  Blocks the main thread. On returning, fetchedObjects will have been deleted.
 *
 *  @param error If there is a problem deleting, upon return contains an instance of NSError that describes the problem.
 *
 *  @return YES if the delete succeeds, otherwise NO.
 */
- (BOOL)deleteObjects:(NSError**)error;

/**
 *  Perform a deletion in a background thread.
 *  Provide a delegate or set deletedObjectsBlock to perfom an action once the objects are deleted.
 */
- (void)deleteObjectsAsynchronously;

@end