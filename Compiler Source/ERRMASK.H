/****************************************************************************/
/*                           COPYRIGHT NOTICE                               */
/****************************************************************************/
/* Vulkan CONFIDENTIAL INFORMATION:                           Category:  1  */
/* COPYRIGHT (c) 1990 Vulkan Technologies Ltd.                              */
/*                                                                          */
/* This data file contains confidential and proprietary information of      */
/* Vulkan Technologies Ltd, and any reproduction, disclosure, or other use  */
/* in whole or in part, is hereby expressly prohibited.                     */
/*                                                                          */
/* This restriction applies unless, prior written agreement or prior        */
/* written permission has been given by Vulkan Technologies Ltd.            */
/*                                                                          */
/* Unlawful use of this material shall render the using party liable to     */
/* prosecution.                                                             */
/*                                                                          */
/****************************************************************************/

struct err_msg {
       short          severity;      /* 2 bytes   */
       short          length;        /* 2 bytes   */
       char           text[508];     /* 512 bytes */
       } message;

