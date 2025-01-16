#/***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 2021
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************/

# Message type identifiers. Used in inter-process socket communication
#
MSG_RESPONSE = 1                # Generic response message - symantics defined by the request type
MSG_LOAD_EXTENSION = 10         # Loads an extension from its registered location
MSG_GET_NAME = 11               # Retrieve the name of the extension
MSG_GET_VERSION = 12            # Retrieves the version of the component engine that this extension works with
MSG_GET_PROPERTY_SPEC = 13      # Retrieves the property spec definition for the current extension
MSG_GET_SYNTAX_SPEC = 14        # Retrieves the syntax specification for the current extension
MSG_EXECUTE = 15                # Executes the current extension
MSG_CLOSE_PROCESS = 16          # Closes the Python process
MSG_NEXT_RECORD = 20            # Retrieves the next record in a data iteration
MSG_CREATE_VARIABLES = 21       # Create new output variables in the active dataset
MSG_RUN_SYNTAX = 22             # Execute syntax against the central system
MSG_GENERATE_OUTPUT = 23        # Use StatJSON to generate workbook output
MSG_FINISH = 24                 # Inform the backend that the XTension has completed execution and can be closed
