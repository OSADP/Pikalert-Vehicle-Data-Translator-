#!/usr/bin/env python


import os

# ---------------------------------------------------------------------------

def link_dirs(orig, dest):
    """
    This function creates a symbolic link (dest) to the file or
    directory orig, if the destination does not already exist.

    Inputs:
      orig -   original 
      dest -   destination 

    Returns:
      0 - success
      1 - failed
      
    """
    # -----------
    # Check that orig exists and dest does not, then create symlink.
    # -----------
    if ( (os.path.exists(orig) ) and (os.path.exists(dest)==False) ):
        try:
            os.symlink(orig, dest)
            return 0
        except:
            return 1
    else:
        return 1


# ---------------------------------------------------------------------------

def link_dirs_force(orig, dest):
    """
    This function creates a symbolic link (dest) to the file or
    directory orig, removing the destination if it already exists.

    Inputs:
      orig -   original
      dest -   destination
      
    Returns:
      0 - success
      1 - failed
      
    """
    # -----------
    # Check that orig exists, then if dest exists, remove it, make symlink
    # -----------
    if (os.path.exists(orig)):
        try:
            if os.path.exists(dest):
                os.unlink(dest)

            return (link_dirs(orig, dest))
        except:
            return 1
    else:
        return 1
    


