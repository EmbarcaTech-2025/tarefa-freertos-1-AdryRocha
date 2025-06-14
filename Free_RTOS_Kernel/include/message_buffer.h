/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


/*
 * Message buffers build functionality on top of FreeRTOS stream buffers.
 * Whereas stream buffers are used to send a continuous stream of data from one
 * task or interrupt to another, message buffers are used to send variable
 * length discrete messages from one task or interrupt to another.  Their
 * implementation is light weight, making them particularly suited for interrupt
 * to task and core to core communication scenarios.
 *
 * ***NOTE***:  Uniquely among FreeRTOS objects, the stream buffer
 * implementation (so also the message buffer implementation, as message buffers
 * are built on top of stream buffers) assumes there is only one task or
 * interrupt that will write to the buffer (the writer), and only one task or
 * interrupt that will read from the buffer (the reader).  It is safe for the
 * writer and reader to be different tasks or interrupts, but, unlike other
 * FreeRTOS objects, it is not safe to have multiple different writers or
 * multiple different readers.  If there are to be multiple different writers
 * then the application writer must serialize calls to writing API functions
 * (such as xStreamBufferSend()).  Likewise, if there are to be multiple
 * different readers then the application writer must serialize calls to reading
 * API functions (such as xStreamBufferReceive()).  One way to achieve such
 * serialization in single core or SMP kernel is to place each API call inside a
 * critical section and use a block time of 0.
 *
 * Message buffers hold variable length messages.  To enable that, when a
 * message is written to the message buffer an additional sizeof( size_t ) bytes
 * are also written to store the message's length (that happens internally, with
 * the API function).  sizeof( size_t ) is typically 4 bytes on a 32-bit
 * architecture, so writing a 10 byte message to a message buffer on a 32-bit
 * architecture will actually reduce the available space in the message buffer
 * by 14 bytes (10 byte are used by the message, and 4 bytes to hold the length
 * of the message).
 */

#ifndef FREERTOS_MESSAGE_BUFFER_H
#define FREERTOS_MESSAGE_BUFFER_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include message_buffer.h"
#endif

/* Message buffers are built onto of stream buffers. */
#include "stream_buffer.h"

/* *INDENT-OFF* */
#if defined( __cplusplus )
    extern "C" {
#endif
/* *INDENT-ON* */

/**
 * Type by which message buffers are referenced.  For example, a call to
 * xMessageBufferCreate() returns an MessageBufferHandle_t variable that can
 * then be used as a parameter to xMessageBufferSend(), xMessageBufferReceive(),
 * etc. Message buffer is essentially built as a stream buffer hence its handle
 * is also set to same type as a stream buffer handle.
 */
typedef StreamBufferHandle_t MessageBufferHandle_t;

/*-----------------------------------------------------------*/

/**
 * message_buffer.h
 *
 * @code{c}
 * MessageBufferHandle_t xMessageBufferCreate( size_t xBufferSizeBytes );
 * @endcode
 *
 * Creates a new message buffer using dynamically allocated memory.  See
 * xMessageBufferCreateStatic() for a version that uses statically allocated
 * memory (memory that is allocated at compile time).
 *
 * configSUPPORT_DYNAMIC_ALLOCATION must be set to 1 or left undefined in
 * FreeRTOSConfig.h for xMessageBufferCreate() to be available.
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferCreate() to be available.
 *
 * @param xBufferSizeBytes The total number of bytes (not messages) the message
 * buffer will be able to hold at any one time.  When a message is written to
 * the message buffer an additional sizeof( size_t ) bytes are also written to
 * store the message's length.  sizeof( size_t ) is typically 4 bytes on a
 * 32-bit architecture, so on most 32-bit architectures a 10 byte message will
 * take up 14 bytes of message buffer space.
 *
 * @param pxSendCompletedCallback Callback invoked when a send operation to the
 * message buffer is complete. If the parameter is NULL or xMessageBufferCreate()
 * is called without the parameter, then it will use the default implementation
 * provided by sbSEND_COMPLETED macro. To enable the callback,
 * configUSE_SB_COMPLETED_CALLBACK must be set to 1 in FreeRTOSConfig.h.
 *
 * @param pxReceiveCompletedCallback Callback invoked when a receive operation from
 * the message buffer is complete. If the parameter is NULL or xMessageBufferCreate()
 * is called without the parameter, it will use the default implementation provided
 * by sbRECEIVE_COMPLETED macro. To enable the callback,
 * configUSE_SB_COMPLETED_CALLBACK must be set to 1 in FreeRTOSConfig.h.
 *
 * @return If NULL is returned, then the message buffer cannot be created
 * because there is insufficient heap memory available for FreeRTOS to allocate
 * the message buffer data structures and storage area.  A non-NULL value being
 * returned indicates that the message buffer has been created successfully -
 * the returned value should be stored as the handle to the created message
 * buffer.
 *
 * Example use:
 * @code{c}
 *
 * void vAFunction( void )
 * {
 * MessageBufferHandle_t xMessageBuffer;
 * const size_t xMessageBufferSizeBytes = 100;
 *
 *  // Create a message buffer that can hold 100 bytes.  The memory used to hold
 *  // both the message buffer structure and the messages themselves is allocated
 *  // dynamically.  Each message added to the buffer consumes an additional 4
 *  // bytes which are used to hold the length of the message.
 *  xMessageBuffer = xMessageBufferCreate( xMessageBufferSizeBytes );
 *
 *  if( xMessageBuffer == NULL )
 *  {
 *      // There was not enough heap memory space available to create the
 *      // message buffer.
 *  }
 *  else
 *  {
 *      // The message buffer was created successfully and can now be used.
 *  }
 *
 * @endcode
 * \defgroup xMessageBufferCreate xMessageBufferCreate
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferCreate( xBufferSizeBytes ) \
    xStreamBufferGenericCreate( ( xBufferSizeBytes ), ( size_t ) 0, sbTYPE_MESSAGE_BUFFER, NULL, NULL )

#if ( configUSE_SB_COMPLETED_CALLBACK == 1 )
    #define xMessageBufferCreateWithCallback( xBufferSizeBytes, pxSendCompletedCallback, pxReceiveCompletedCallback ) \
    xStreamBufferGenericCreate( ( xBufferSizeBytes ), ( size_t ) 0, sbTYPE_MESSAGE_BUFFER, ( pxSendCompletedCallback ), ( pxReceiveCompletedCallback ) )
#endif

/**
 * message_buffer.h
 *
 * @code{c}
 * MessageBufferHandle_t xMessageBufferCreateStatic( size_t xBufferSizeBytes,
 *                                                   uint8_t *pucMessageBufferStorageArea,
 *                                                   StaticMessageBuffer_t *pxStaticMessageBuffer );
 * @endcode
 * Creates a new message buffer using statically allocated memory.  See
 * xMessageBufferCreate() for a version that uses dynamically allocated memory.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferCreateStatic() to be available.
 *
 * @param xBufferSizeBytes The size, in bytes, of the buffer pointed to by the
 * pucMessageBufferStorageArea parameter.  When a message is written to the
 * message buffer an additional sizeof( size_t ) bytes are also written to store
 * the message's length.  sizeof( size_t ) is typically 4 bytes on a 32-bit
 * architecture, so on most 32-bit architecture a 10 byte message will take up
 * 14 bytes of message buffer space.  The maximum number of bytes that can be
 * stored in the message buffer is actually (xBufferSizeBytes - 1).
 *
 * @param pucMessageBufferStorageArea Must point to a uint8_t array that is at
 * least xBufferSizeBytes big.  This is the array to which messages are
 * copied when they are written to the message buffer.
 *
 * @param pxStaticMessageBuffer Must point to a variable of type
 * StaticMessageBuffer_t, which will be used to hold the message buffer's data
 * structure.
 *
 * @param pxSendCompletedCallback Callback invoked when a new message is sent to the message buffer.
 * If the parameter is NULL or xMessageBufferCreate() is called without the parameter, then it will use the default
 * implementation provided by sbSEND_COMPLETED macro. To enable the callback,
 * configUSE_SB_COMPLETED_CALLBACK must be set to 1 in FreeRTOSConfig.h.
 *
 * @param pxReceiveCompletedCallback Callback invoked when a message is read from a
 * message buffer. If the parameter is NULL or xMessageBufferCreate() is called without the parameter, it will
 * use the default implementation provided by sbRECEIVE_COMPLETED macro. To enable the callback,
 * configUSE_SB_COMPLETED_CALLBACK must be set to 1 in FreeRTOSConfig.h.
 *
 * @return If the message buffer is created successfully then a handle to the
 * created message buffer is returned. If either pucMessageBufferStorageArea or
 * pxStaticmessageBuffer are NULL then NULL is returned.
 *
 * Example use:
 * @code{c}
 *
 * // Used to dimension the array used to hold the messages.  The available space
 * // will actually be one less than this, so 999.
 #define STORAGE_SIZE_BYTES 1000
 *
 * // Defines the memory that will actually hold the messages within the message
 * // buffer.
 * static uint8_t ucStorageBuffer[ STORAGE_SIZE_BYTES ];
 *
 * // The variable used to hold the message buffer structure.
 * StaticMessageBuffer_t xMessageBufferStruct;
 *
 * void MyFunction( void )
 * {
 * MessageBufferHandle_t xMessageBuffer;
 *
 *  xMessageBuffer = xMessageBufferCreateStatic( sizeof( ucStorageBuffer ),
 *                                               ucStorageBuffer,
 *                                               &xMessageBufferStruct );
 *
 *  // As neither the pucMessageBufferStorageArea or pxStaticMessageBuffer
 *  // parameters were NULL, xMessageBuffer will not be NULL, and can be used to
 *  // reference the created message buffer in other message buffer API calls.
 *
 *  // Other code that uses the message buffer can go here.
 * }
 *
 * @endcode
 * \defgroup xMessageBufferCreateStatic xMessageBufferCreateStatic
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferCreateStatic( xBufferSizeBytes, pucMessageBufferStorageArea, pxStaticMessageBuffer ) \
    xStreamBufferGenericCreateStatic( ( xBufferSizeBytes ), 0, sbTYPE_MESSAGE_BUFFER, ( pucMessageBufferStorageArea ), ( pxStaticMessageBuffer ), NULL, NULL )

#if ( configUSE_SB_COMPLETED_CALLBACK == 1 )
    #define xMessageBufferCreateStaticWithCallback( xBufferSizeBytes, pucMessageBufferStorageArea, pxStaticMessageBuffer, pxSendCompletedCallback, pxReceiveCompletedCallback ) \
    xStreamBufferGenericCreateStatic( ( xBufferSizeBytes ), 0, sbTYPE_MESSAGE_BUFFER, ( pucMessageBufferStorageArea ), ( pxStaticMessageBuffer ), ( pxSendCompletedCallback ), ( pxReceiveCompletedCallback ) )
#endif

/**
 * message_buffer.h
 *
 * @code{c}
 * BaseType_t xMessageBufferGetStaticBuffers( MessageBufferHandle_t xMessageBuffer,
 *                                            uint8_t ** ppucMessageBufferStorageArea,
 *                                            StaticMessageBuffer_t ** ppxStaticMessageBuffer );
 * @endcode
 *
 * Retrieve pointers to a statically created message buffer's data structure
 * buffer and storage area buffer. These are the same buffers that are supplied
 * at the time of creation.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferGetStaticBuffers() to be available.
 *
 * @param xMessageBuffer The message buffer for which to retrieve the buffers.
 *
 * @param ppucMessageBufferStorageArea Used to return a pointer to the
 * message buffer's storage area buffer.
 *
 * @param ppxStaticMessageBuffer Used to return a pointer to the message
 * buffer's data structure buffer.
 *
 * @return pdTRUE if buffers were retrieved, pdFALSE otherwise..
 *
 * \defgroup xMessageBufferGetStaticBuffers xMessageBufferGetStaticBuffers
 * \ingroup MessageBufferManagement
 */
#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    #define xMessageBufferGetStaticBuffers( xMessageBuffer, ppucMessageBufferStorageArea, ppxStaticMessageBuffer ) \
    xStreamBufferGetStaticBuffers( ( xMessageBuffer ), ( ppucMessageBufferStorageArea ), ( ppxStaticMessageBuffer ) )
#endif /* configSUPPORT_STATIC_ALLOCATION */

/**
 * message_buffer.h
 *
 * @code{c}
 * size_t xMessageBufferSend( MessageBufferHandle_t xMessageBuffer,
 *                            const void *pvTxData,
 *                            size_t xDataLengthBytes,
 *                            TickType_t xTicksToWait );
 * @endcode
 *
 * Sends a discrete message to the message buffer.  The message can be any
 * length that fits within the buffer's free space, and is copied into the
 * buffer.
 *
 * ***NOTE***:  Uniquely among FreeRTOS objects, the stream buffer
 * implementation (so also the message buffer implementation, as message buffers
 * are built on top of stream buffers) assumes there is only one task or
 * interrupt that will write to the buffer (the writer), and only one task or
 * interrupt that will read from the buffer (the reader).  It is safe for the
 * writer and reader to be different tasks or interrupts, but, unlike other
 * FreeRTOS objects, it is not safe to have multiple different writers or
 * multiple different readers.  If there are to be multiple different writers
 * then the application writer must serialize calls to writing API functions
 * (such as xStreamBufferSend()).  Likewise, if there are to be multiple
 * different readers then the application writer must serialize calls to reading
 * API functions (such as xStreamBufferReceive()).  One way to achieve such
 * serialization in single core or SMP kernel is to place each API call inside a
 * critical section and use a block time of 0.
 *
 * Use xMessageBufferSend() to write to a message buffer from a task.  Use
 * xMessageBufferSendFromISR() to write to a message buffer from an interrupt
 * service routine (ISR).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferSend() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer to which a message is
 * being sent.
 *
 * @param pvTxData A pointer to the message that is to be copied into the
 * message buffer.
 *
 * @param xDataLengthBytes The length of the message.  That is, the number of
 * bytes to copy from pvTxData into the message buffer.  When a message is
 * written to the message buffer an additional sizeof( size_t ) bytes are also
 * written to store the message's length.  sizeof( size_t ) is typically 4 bytes
 * on a 32-bit architecture, so on most 32-bit architecture setting
 * xDataLengthBytes to 20 will reduce the free space in the message buffer by 24
 * bytes (20 bytes of message data and 4 bytes to hold the message length).
 *
 * @param xTicksToWait The maximum amount of time the calling task should remain
 * in the Blocked state to wait for enough space to become available in the
 * message buffer, should the message buffer have insufficient space when
 * xMessageBufferSend() is called.  The calling task will never block if
 * xTicksToWait is zero.  The block time is specified in tick periods, so the
 * absolute time it represents is dependent on the tick frequency.  The macro
 * pdMS_TO_TICKS() can be used to convert a time specified in milliseconds into
 * a time specified in ticks.  Setting xTicksToWait to portMAX_DELAY will cause
 * the task to wait indefinitely (without timing out), provided
 * INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h.  Tasks do not use any
 * CPU time when they are in the Blocked state.
 *
 * @return The number of bytes written to the message buffer.  If the call to
 * xMessageBufferSend() times out before there was enough space to write the
 * message into the message buffer then zero is returned.  If the call did not
 * time out then xDataLengthBytes is returned.
 *
 * Example use:
 * @code{c}
 * void vAFunction( MessageBufferHandle_t xMessageBuffer )
 * {
 * size_t xBytesSent;
 * uint8_t ucArrayToSend[] = { 0, 1, 2, 3 };
 * char *pcStringToSend = "String to send";
 * const TickType_t x100ms = pdMS_TO_TICKS( 100 );
 *
 *  // Send an array to the message buffer, blocking for a maximum of 100ms to
 *  // wait for enough space to be available in the message buffer.
 *  xBytesSent = xMessageBufferSend( xMessageBuffer, ( void * ) ucArrayToSend, sizeof( ucArrayToSend ), x100ms );
 *
 *  if( xBytesSent != sizeof( ucArrayToSend ) )
 *  {
 *      // The call to xMessageBufferSend() times out before there was enough
 *      // space in the buffer for the data to be written.
 *  }
 *
 *  // Send the string to the message buffer.  Return immediately if there is
 *  // not enough space in the buffer.
 *  xBytesSent = xMessageBufferSend( xMessageBuffer, ( void * ) pcStringToSend, strlen( pcStringToSend ), 0 );
 *
 *  if( xBytesSent != strlen( pcStringToSend ) )
 *  {
 *      // The string could not be added to the message buffer because there was
 *      // not enough free space in the buffer.
 *  }
 * }
 * @endcode
 * \defgroup xMessageBufferSend xMessageBufferSend
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferSend( xMessageBuffer, pvTxData, xDataLengthBytes, xTicksToWait ) \
    xStreamBufferSend( ( xMessageBuffer ), ( pvTxData ), ( xDataLengthBytes ), ( xTicksToWait ) )

/**
 * message_buffer.h
 *
 * @code{c}
 * size_t xMessageBufferSendFromISR( MessageBufferHandle_t xMessageBuffer,
 *                                   const void *pvTxData,
 *                                   size_t xDataLengthBytes,
 *                                   BaseType_t *pxHigherPriorityTaskWoken );
 * @endcode
 *
 * Interrupt safe version of the API function that sends a discrete message to
 * the message buffer.  The message can be any length that fits within the
 * buffer's free space, and is copied into the buffer.
 *
 * ***NOTE***:  Uniquely among FreeRTOS objects, the stream buffer
 * implementation (so also the message buffer implementation, as message buffers
 * are built on top of stream buffers) assumes there is only one task or
 * interrupt that will write to the buffer (the writer), and only one task or
 * interrupt that will read from the buffer (the reader).  It is safe for the
 * writer and reader to be different tasks or interrupts, but, unlike other
 * FreeRTOS objects, it is not safe to have multiple different writers or
 * multiple different readers.  If there are to be multiple different writers
 * then the application writer must serialize calls to writing API functions
 * (such as xStreamBufferSend()).  Likewise, if there are to be multiple
 * different readers then the application writer must serialize calls to reading
 * API functions (such as xStreamBufferReceive()).  One way to achieve such
 * serialization in single core or SMP kernel is to place each API call inside a
 * critical section and use a block time of 0.
 *
 * Use xMessageBufferSend() to write to a message buffer from a task.  Use
 * xMessageBufferSendFromISR() to write to a message buffer from an interrupt
 * service routine (ISR).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferSendFromISR() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer to which a message is
 * being sent.
 *
 * @param pvTxData A pointer to the message that is to be copied into the
 * message buffer.
 *
 * @param xDataLengthBytes The length of the message.  That is, the number of
 * bytes to copy from pvTxData into the message buffer.  When a message is
 * written to the message buffer an additional sizeof( size_t ) bytes are also
 * written to store the message's length.  sizeof( size_t ) is typically 4 bytes
 * on a 32-bit architecture, so on most 32-bit architecture setting
 * xDataLengthBytes to 20 will reduce the free space in the message buffer by 24
 * bytes (20 bytes of message data and 4 bytes to hold the message length).
 *
 * @param pxHigherPriorityTaskWoken  It is possible that a message buffer will
 * have a task blocked on it waiting for data.  Calling
 * xMessageBufferSendFromISR() can make data available, and so cause a task that
 * was waiting for data to leave the Blocked state.  If calling
 * xMessageBufferSendFromISR() causes a task to leave the Blocked state, and the
 * unblocked task has a priority higher than the currently executing task (the
 * task that was interrupted), then, internally, xMessageBufferSendFromISR()
 * will set *pxHigherPriorityTaskWoken to pdTRUE.  If
 * xMessageBufferSendFromISR() sets this value to pdTRUE, then normally a
 * context switch should be performed before the interrupt is exited.  This will
 * ensure that the interrupt returns directly to the highest priority Ready
 * state task.  *pxHigherPriorityTaskWoken should be set to pdFALSE before it
 * is passed into the function.  See the code example below for an example.
 *
 * @return The number of bytes actually written to the message buffer.  If the
 * message buffer didn't have enough free space for the message to be stored
 * then 0 is returned, otherwise xDataLengthBytes is returned.
 *
 * Example use:
 * @code{c}
 * // A message buffer that has already been created.
 * MessageBufferHandle_t xMessageBuffer;
 *
 * void vAnInterruptServiceRoutine( void )
 * {
 * size_t xBytesSent;
 * char *pcStringToSend = "String to send";
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE; // Initialised to pdFALSE.
 *
 *  // Attempt to send the string to the message buffer.
 *  xBytesSent = xMessageBufferSendFromISR( xMessageBuffer,
 *                                          ( void * ) pcStringToSend,
 *                                          strlen( pcStringToSend ),
 *                                          &xHigherPriorityTaskWoken );
 *
 *  if( xBytesSent != strlen( pcStringToSend ) )
 *  {
 *      // The string could not be added to the message buffer because there was
 *      // not enough free space in the buffer.
 *  }
 *
 *  // If xHigherPriorityTaskWoken was set to pdTRUE inside
 *  // xMessageBufferSendFromISR() then a task that has a priority above the
 *  // priority of the currently executing task was unblocked and a context
 *  // switch should be performed to ensure the ISR returns to the unblocked
 *  // task.  In most FreeRTOS ports this is done by simply passing
 *  // xHigherPriorityTaskWoken into portYIELD_FROM_ISR(), which will test the
 *  // variables value, and perform the context switch if necessary.  Check the
 *  // documentation for the port in use for port specific instructions.
 *  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 * }
 * @endcode
 * \defgroup xMessageBufferSendFromISR xMessageBufferSendFromISR
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferSendFromISR( xMessageBuffer, pvTxData, xDataLengthBytes, pxHigherPriorityTaskWoken ) \
    xStreamBufferSendFromISR( ( xMessageBuffer ), ( pvTxData ), ( xDataLengthBytes ), ( pxHigherPriorityTaskWoken ) )

/**
 * message_buffer.h
 *
 * @code{c}
 * size_t xMessageBufferReceive( MessageBufferHandle_t xMessageBuffer,
 *                               void *pvRxData,
 *                               size_t xBufferLengthBytes,
 *                               TickType_t xTicksToWait );
 * @endcode
 *
 * Receives a discrete message from a message buffer.  Messages can be of
 * variable length and are copied out of the buffer.
 *
 * ***NOTE***:  Uniquely among FreeRTOS objects, the stream buffer
 * implementation (so also the message buffer implementation, as message buffers
 * are built on top of stream buffers) assumes there is only one task or
 * interrupt that will write to the buffer (the writer), and only one task or
 * interrupt that will read from the buffer (the reader).  It is safe for the
 * writer and reader to be different tasks or interrupts, but, unlike other
 * FreeRTOS objects, it is not safe to have multiple different writers or
 * multiple different readers.  If there are to be multiple different writers
 * then the application writer must serialize calls to writing API functions
 * (such as xStreamBufferSend()).  Likewise, if there are to be multiple
 * different readers then the application writer must serialize calls to reading
 * API functions (such as xStreamBufferReceive()).  One way to achieve such
 * serialization in single core or SMP kernel is to place each API call inside a
 * critical section and use a block time of 0.
 *
 * Use xMessageBufferReceive() to read from a message buffer from a task.  Use
 * xMessageBufferReceiveFromISR() to read from a message buffer from an
 * interrupt service routine (ISR).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferReceive() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer from which a message
 * is being received.
 *
 * @param pvRxData A pointer to the buffer into which the received message is
 * to be copied.
 *
 * @param xBufferLengthBytes The length of the buffer pointed to by the pvRxData
 * parameter.  This sets the maximum length of the message that can be received.
 * If xBufferLengthBytes is too small to hold the next message then the message
 * will be left in the message buffer and 0 will be returned.
 *
 * @param xTicksToWait The maximum amount of time the task should remain in the
 * Blocked state to wait for a message, should the message buffer be empty.
 * xMessageBufferReceive() will return immediately if xTicksToWait is zero and
 * the message buffer is empty.  The block time is specified in tick periods, so
 * the absolute time it represents is dependent on the tick frequency.  The
 * macro pdMS_TO_TICKS() can be used to convert a time specified in milliseconds
 * into a time specified in ticks.  Setting xTicksToWait to portMAX_DELAY will
 * cause the task to wait indefinitely (without timing out), provided
 * INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h.  Tasks do not use any
 * CPU time when they are in the Blocked state.
 *
 * @return The length, in bytes, of the message read from the message buffer, if
 * any.  If xMessageBufferReceive() times out before a message became available
 * then zero is returned.  If the length of the message is greater than
 * xBufferLengthBytes then the message will be left in the message buffer and
 * zero is returned.
 *
 * Example use:
 * @code{c}
 * void vAFunction( MessageBuffer_t xMessageBuffer )
 * {
 * uint8_t ucRxData[ 20 ];
 * size_t xReceivedBytes;
 * const TickType_t xBlockTime = pdMS_TO_TICKS( 20 );
 *
 *  // Receive the next message from the message buffer.  Wait in the Blocked
 *  // state (so not using any CPU processing time) for a maximum of 100ms for
 *  // a message to become available.
 *  xReceivedBytes = xMessageBufferReceive( xMessageBuffer,
 *                                          ( void * ) ucRxData,
 *                                          sizeof( ucRxData ),
 *                                          xBlockTime );
 *
 *  if( xReceivedBytes > 0 )
 *  {
 *      // A ucRxData contains a message that is xReceivedBytes long.  Process
 *      // the message here....
 *  }
 * }
 * @endcode
 * \defgroup xMessageBufferReceive xMessageBufferReceive
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferReceive( xMessageBuffer, pvRxData, xBufferLengthBytes, xTicksToWait ) \
    xStreamBufferReceive( ( xMessageBuffer ), ( pvRxData ), ( xBufferLengthBytes ), ( xTicksToWait ) )


/**
 * message_buffer.h
 *
 * @code{c}
 * size_t xMessageBufferReceiveFromISR( MessageBufferHandle_t xMessageBuffer,
 *                                      void *pvRxData,
 *                                      size_t xBufferLengthBytes,
 *                                      BaseType_t *pxHigherPriorityTaskWoken );
 * @endcode
 *
 * An interrupt safe version of the API function that receives a discrete
 * message from a message buffer.  Messages can be of variable length and are
 * copied out of the buffer.
 *
 * ***NOTE***:  Uniquely among FreeRTOS objects, the stream buffer
 * implementation (so also the message buffer implementation, as message buffers
 * are built on top of stream buffers) assumes there is only one task or
 * interrupt that will write to the buffer (the writer), and only one task or
 * interrupt that will read from the buffer (the reader).  It is safe for the
 * writer and reader to be different tasks or interrupts, but, unlike other
 * FreeRTOS objects, it is not safe to have multiple different writers or
 * multiple different readers.  If there are to be multiple different writers
 * then the application writer must serialize calls to writing API functions
 * (such as xStreamBufferSend()).  Likewise, if there are to be multiple
 * different readers then the application writer must serialize calls to reading
 * API functions (such as xStreamBufferReceive()).  One way to achieve such
 * serialization in single core or SMP kernel is to place each API call inside a
 * critical section and use a block time of 0.
 *
 * Use xMessageBufferReceive() to read from a message buffer from a task.  Use
 * xMessageBufferReceiveFromISR() to read from a message buffer from an
 * interrupt service routine (ISR).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferReceiveFromISR() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer from which a message
 * is being received.
 *
 * @param pvRxData A pointer to the buffer into which the received message is
 * to be copied.
 *
 * @param xBufferLengthBytes The length of the buffer pointed to by the pvRxData
 * parameter.  This sets the maximum length of the message that can be received.
 * If xBufferLengthBytes is too small to hold the next message then the message
 * will be left in the message buffer and 0 will be returned.
 *
 * @param pxHigherPriorityTaskWoken  It is possible that a message buffer will
 * have a task blocked on it waiting for space to become available.  Calling
 * xMessageBufferReceiveFromISR() can make space available, and so cause a task
 * that is waiting for space to leave the Blocked state.  If calling
 * xMessageBufferReceiveFromISR() causes a task to leave the Blocked state, and
 * the unblocked task has a priority higher than the currently executing task
 * (the task that was interrupted), then, internally,
 * xMessageBufferReceiveFromISR() will set *pxHigherPriorityTaskWoken to pdTRUE.
 * If xMessageBufferReceiveFromISR() sets this value to pdTRUE, then normally a
 * context switch should be performed before the interrupt is exited.  That will
 * ensure the interrupt returns directly to the highest priority Ready state
 * task.  *pxHigherPriorityTaskWoken should be set to pdFALSE before it is
 * passed into the function.  See the code example below for an example.
 *
 * @return The length, in bytes, of the message read from the message buffer, if
 * any.
 *
 * Example use:
 * @code{c}
 * // A message buffer that has already been created.
 * MessageBuffer_t xMessageBuffer;
 *
 * void vAnInterruptServiceRoutine( void )
 * {
 * uint8_t ucRxData[ 20 ];
 * size_t xReceivedBytes;
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // Initialised to pdFALSE.
 *
 *  // Receive the next message from the message buffer.
 *  xReceivedBytes = xMessageBufferReceiveFromISR( xMessageBuffer,
 *                                                ( void * ) ucRxData,
 *                                                sizeof( ucRxData ),
 *                                                &xHigherPriorityTaskWoken );
 *
 *  if( xReceivedBytes > 0 )
 *  {
 *      // A ucRxData contains a message that is xReceivedBytes long.  Process
 *      // the message here....
 *  }
 *
 *  // If xHigherPriorityTaskWoken was set to pdTRUE inside
 *  // xMessageBufferReceiveFromISR() then a task that has a priority above the
 *  // priority of the currently executing task was unblocked and a context
 *  // switch should be performed to ensure the ISR returns to the unblocked
 *  // task.  In most FreeRTOS ports this is done by simply passing
 *  // xHigherPriorityTaskWoken into portYIELD_FROM_ISR(), which will test the
 *  // variables value, and perform the context switch if necessary.  Check the
 *  // documentation for the port in use for port specific instructions.
 *  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 * }
 * @endcode
 * \defgroup xMessageBufferReceiveFromISR xMessageBufferReceiveFromISR
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferReceiveFromISR( xMessageBuffer, pvRxData, xBufferLengthBytes, pxHigherPriorityTaskWoken ) \
    xStreamBufferReceiveFromISR( ( xMessageBuffer ), ( pvRxData ), ( xBufferLengthBytes ), ( pxHigherPriorityTaskWoken ) )

/**
 * message_buffer.h
 *
 * @code{c}
 * void vMessageBufferDelete( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 *
 * Deletes a message buffer that was previously created using a call to
 * xMessageBufferCreate() or xMessageBufferCreateStatic().  If the message
 * buffer was created using dynamic memory (that is, by xMessageBufferCreate()),
 * then the allocated memory is freed.
 *
 * A message buffer handle must not be used after the message buffer has been
 * deleted.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * vMessageBufferDelete() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer to be deleted.
 *
 */
#define vMessageBufferDelete( xMessageBuffer ) \
    vStreamBufferDelete( xMessageBuffer )

/**
 * message_buffer.h
 * @code{c}
 * BaseType_t xMessageBufferIsFull( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 *
 * Tests to see if a message buffer is full.  A message buffer is full if it
 * cannot accept any more messages, of any size, until space is made available
 * by a message being removed from the message buffer.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferIsFull() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer being queried.
 *
 * @return If the message buffer referenced by xMessageBuffer is full then
 * pdTRUE is returned.  Otherwise pdFALSE is returned.
 */
#define xMessageBufferIsFull( xMessageBuffer ) \
    xStreamBufferIsFull( xMessageBuffer )

/**
 * message_buffer.h
 * @code{c}
 * BaseType_t xMessageBufferIsEmpty( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 *
 * Tests to see if a message buffer is empty (does not contain any messages).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferIsEmpty() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer being queried.
 *
 * @return If the message buffer referenced by xMessageBuffer is empty then
 * pdTRUE is returned.  Otherwise pdFALSE is returned.
 *
 */
#define xMessageBufferIsEmpty( xMessageBuffer ) \
    xStreamBufferIsEmpty( xMessageBuffer )

/**
 * message_buffer.h
 * @code{c}
 * BaseType_t xMessageBufferReset( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 *
 * Resets a message buffer to its initial empty state, discarding any message it
 * contained.
 *
 * A message buffer can only be reset if there are no tasks blocked on it.
 *
 * Use xMessageBufferReset() to reset a message buffer from a task.
 * Use xMessageBufferResetFromISR() to reset a message buffer from an
 * interrupt service routine (ISR).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferReset() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer being reset.
 *
 * @return If the message buffer was reset then pdPASS is returned.  If the
 * message buffer could not be reset because either there was a task blocked on
 * the message queue to wait for space to become available, or to wait for a
 * a message to be available, then pdFAIL is returned.
 *
 * \defgroup xMessageBufferReset xMessageBufferReset
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferReset( xMessageBuffer ) \
    xStreamBufferReset( xMessageBuffer )


/**
 * message_buffer.h
 * @code{c}
 * BaseType_t xMessageBufferResetFromISR( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 *
 * An interrupt safe version of the API function that resets the message buffer.
 * Resets a message buffer to its initial empty state, discarding any message it
 * contained.
 *
 * A message buffer can only be reset if there are no tasks blocked on it.
 *
 * Use xMessageBufferReset() to reset a message buffer from a task.
 * Use xMessageBufferResetFromISR() to reset a message buffer from an
 * interrupt service routine (ISR).
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferResetFromISR() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer being reset.
 *
 * @return If the message buffer was reset then pdPASS is returned.  If the
 * message buffer could not be reset because either there was a task blocked on
 * the message queue to wait for space to become available, or to wait for a
 * a message to be available, then pdFAIL is returned.
 *
 * \defgroup xMessageBufferResetFromISR xMessageBufferResetFromISR
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferResetFromISR( xMessageBuffer ) \
    xStreamBufferResetFromISR( xMessageBuffer )

/**
 * message_buffer.h
 * @code{c}
 * size_t xMessageBufferSpaceAvailable( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 * Returns the number of bytes of free space in the message buffer.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferSpaceAvailable() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer being queried.
 *
 * @return The number of bytes that can be written to the message buffer before
 * the message buffer would be full.  When a message is written to the message
 * buffer an additional sizeof( size_t ) bytes are also written to store the
 * message's length.  sizeof( size_t ) is typically 4 bytes on a 32-bit
 * architecture, so if xMessageBufferSpacesAvailable() returns 10, then the size
 * of the largest message that can be written to the message buffer is 6 bytes.
 *
 * \defgroup xMessageBufferSpaceAvailable xMessageBufferSpaceAvailable
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferSpaceAvailable( xMessageBuffer ) \
    xStreamBufferSpacesAvailable( xMessageBuffer )
#define xMessageBufferSpacesAvailable( xMessageBuffer ) \
    xStreamBufferSpacesAvailable( xMessageBuffer ) /* Corrects typo in original macro name. */

/**
 * message_buffer.h
 * @code{c}
 * size_t xMessageBufferNextLengthBytes( MessageBufferHandle_t xMessageBuffer );
 * @endcode
 * Returns the length (in bytes) of the next message in a message buffer.
 * Useful if xMessageBufferReceive() returned 0 because the size of the buffer
 * passed into xMessageBufferReceive() was too small to hold the next message.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferNextLengthBytes() to be available.
 *
 * @param xMessageBuffer The handle of the message buffer being queried.
 *
 * @return The length (in bytes) of the next message in the message buffer, or 0
 * if the message buffer is empty.
 *
 * \defgroup xMessageBufferNextLengthBytes xMessageBufferNextLengthBytes
 * \ingroup MessageBufferManagement
 */
#define xMessageBufferNextLengthBytes( xMessageBuffer ) \
    xStreamBufferNextMessageLengthBytes( xMessageBuffer )

/**
 * message_buffer.h
 *
 * @code{c}
 * BaseType_t xMessageBufferSendCompletedFromISR( MessageBufferHandle_t xMessageBuffer, BaseType_t *pxHigherPriorityTaskWoken );
 * @endcode
 *
 * For advanced users only.
 *
 * The sbSEND_COMPLETED() macro is called from within the FreeRTOS APIs when
 * data is sent to a message buffer or stream buffer.  If there was a task that
 * was blocked on the message or stream buffer waiting for data to arrive then
 * the sbSEND_COMPLETED() macro sends a notification to the task to remove it
 * from the Blocked state.  xMessageBufferSendCompletedFromISR() does the same
 * thing.  It is provided to enable application writers to implement their own
 * version of sbSEND_COMPLETED(), and MUST NOT BE USED AT ANY OTHER TIME.
 *
 * See the example implemented in FreeRTOS/Demo/Minimal/MessageBufferAMP.c for
 * additional information.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferSendCompletedFromISR() to be available.
 *
 * @param xMessageBuffer The handle of the stream buffer to which data was
 * written.
 *
 * @param pxHigherPriorityTaskWoken *pxHigherPriorityTaskWoken should be
 * initialised to pdFALSE before it is passed into
 * xMessageBufferSendCompletedFromISR().  If calling
 * xMessageBufferSendCompletedFromISR() removes a task from the Blocked state,
 * and the task has a priority above the priority of the currently running task,
 * then *pxHigherPriorityTaskWoken will get set to pdTRUE indicating that a
 * context switch should be performed before exiting the ISR.
 *
 * @return If a task was removed from the Blocked state then pdTRUE is returned.
 * Otherwise pdFALSE is returned.
 *
 * \defgroup xMessageBufferSendCompletedFromISR xMessageBufferSendCompletedFromISR
 * \ingroup StreamBufferManagement
 */
#define xMessageBufferSendCompletedFromISR( xMessageBuffer, pxHigherPriorityTaskWoken ) \
    xStreamBufferSendCompletedFromISR( ( xMessageBuffer ), ( pxHigherPriorityTaskWoken ) )

/**
 * message_buffer.h
 *
 * @code{c}
 * BaseType_t xMessageBufferReceiveCompletedFromISR( MessageBufferHandle_t xMessageBuffer, BaseType_t *pxHigherPriorityTaskWoken );
 * @endcode
 *
 * For advanced users only.
 *
 * The sbRECEIVE_COMPLETED() macro is called from within the FreeRTOS APIs when
 * data is read out of a message buffer or stream buffer.  If there was a task
 * that was blocked on the message or stream buffer waiting for data to arrive
 * then the sbRECEIVE_COMPLETED() macro sends a notification to the task to
 * remove it from the Blocked state.  xMessageBufferReceiveCompletedFromISR()
 * does the same thing.  It is provided to enable application writers to
 * implement their own version of sbRECEIVE_COMPLETED(), and MUST NOT BE USED AT
 * ANY OTHER TIME.
 *
 * See the example implemented in FreeRTOS/Demo/Minimal/MessageBufferAMP.c for
 * additional information.
 *
 * configUSE_STREAM_BUFFERS must be set to 1 in for FreeRTOSConfig.h for
 * xMessageBufferReceiveCompletedFromISR() to be available.
 *
 * @param xMessageBuffer The handle of the stream buffer from which data was
 * read.
 *
 * @param pxHigherPriorityTaskWoken *pxHigherPriorityTaskWoken should be
 * initialised to pdFALSE before it is passed into
 * xMessageBufferReceiveCompletedFromISR().  If calling
 * xMessageBufferReceiveCompletedFromISR() removes a task from the Blocked state,
 * and the task has a priority above the priority of the currently running task,
 * then *pxHigherPriorityTaskWoken will get set to pdTRUE indicating that a
 * context switch should be performed before exiting the ISR.
 *
 * @return If a task was removed from the Blocked state then pdTRUE is returned.
 * Otherwise pdFALSE is returned.
 *
 * \defgroup xMessageBufferReceiveCompletedFromISR xMessageBufferReceiveCompletedFromISR
 * \ingroup StreamBufferManagement
 */
#define xMessageBufferReceiveCompletedFromISR( xMessageBuffer, pxHigherPriorityTaskWoken ) \
    xStreamBufferReceiveCompletedFromISR( ( xMessageBuffer ), ( pxHigherPriorityTaskWoken ) )

/* *INDENT-OFF* */
#if defined( __cplusplus )
    } /* extern "C" */
#endif
/* *INDENT-ON* */

#endif /* !defined( FREERTOS_MESSAGE_BUFFER_H ) */
