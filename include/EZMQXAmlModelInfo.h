/**
* @file
*
* This file provide enum types for Aml Data model.
*/

#ifndef EZMQ_X_MODEL_INFO_H
#define EZMQ_X_MODEL_INFO_H

namespace EZMQX
{
    /**
    * @enum AmlModelInfo
    * Information of types for AML Model info.
    */
    typedef enum
    {
        AmlModelId,    /**< enum ID of AmlModel. */
        AmlFilePath    /**< enum File path of *.aml file. */
    }AmlModelInfo;
}

#endif //EZMQ_X_MODEL_INFO_H