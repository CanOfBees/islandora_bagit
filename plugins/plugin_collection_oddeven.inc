<?php

/**
 * @file
 * Plugin for the Islandora BagIt Drupal module.
 *
 * Within the Bag created for the collection, arranges child objects
 * into 'odd' or 'even' subdirectories based on their PID, and then
 * within those directories, in their own directories as illustrated
 * below.
 * 
 * More of a demonstration of an alternative arrangement for objects
 * within a Bag than anything else.
 */

/**
 * Returns an array of source and destination file paths. Returns FALSE if no 
 * datastream files are present.
 *
 * @param object $islandora_object
 *   The Islandora object to create a Bag for.
 * 
 * @param array $files_to_add
 *   The list of files generated by the object plugins.
 * 
 * @param string $tmp_ds_directory
 *  The temporary directory where the datastream files have been downloaded.
 * 
 * @return string
 */
function islandora_bagit_plugin_collection_oddeven_init($islandora_object, $files_to_add, $tmp_ds_directory) {
/*
 * Arranges objects in two folders in the Bag's 'data' directory, 'odd' and 'even',
 * based on each object's PID.
 *
 * mybag/
 *   data/
 *      odd/
 *       object_90/
 *       object_92/
 *      even/
 *        object_89/
 *        object_91/
 *           DS1.tif
 *           DS2.jpg
 *           DS3.xml
 *           somefile.xml
 */
 
  if (!count($files_to_add)) {
    return FALSE;
  }
  
  // Sanitize the object's PID so it can be used in file paths.
  $clean_pid = str_replace(array(':','-'), '_', $islandora_object->id);

  // Create the 'odd' and 'even' directories.
  $odd_dir = $tmp_ds_directory . DIRECTORY_SEPARATOR . 'odd';
  if (!file_exists($odd_dir)) {
    mkdir($odd_dir, 0777, TRUE);
  }  
  $even_dir = $tmp_ds_directory . DIRECTORY_SEPARATOR . 'even';
  if (!file_exists($even_dir)) {
    mkdir($even_dir, 0777, TRUE);
  }  
  
  // Within the 'odd' and 'even' directories, we create a directory for
  // each object, as illustrated above.
  $which_dir = '';
  if (preg_match('/[02468]$/', $clean_pid)) {
    $which_dir = 'even';
  }
  if (preg_match('/[13579]$/', $clean_pid)) {
    $which_dir = 'odd';
  }

  $object_tmp_dir = $tmp_ds_directory . DIRECTORY_SEPARATOR . $which_dir . DIRECTORY_SEPARATOR . $clean_pid;
  // Create the object-level directory.
  if (!file_exists($object_tmp_dir)) {
    mkdir($object_tmp_dir, 0777, TRUE);
  }  
  
  // For each of the files in $files_to_add, 1) move each file from its
  // original location to the new object-level directories and 2) modify
  // the $files_to_add array so that the source and dest include the
  // object-level directories.
  foreach ($files_to_add as &$file) {
      $object_source_path = $object_tmp_dir . DIRECTORY_SEPARATOR . basename($file['source']);
      // Move the file from its current location in $tmp_ds_directory to its new
      // object-specific location in $object_tmp_dir.
      rename($file['source'], $object_source_path);
      // Modify the source and destination paths for each file so they can be
      // passed back to the main module to be added to the Bag.
      $file['source'] = $object_source_path;
      // $file['dest'] is relative to the Bag's 'data' directory, as illustrated above.
      $file['dest'] = $which_dir . DIRECTORY_SEPARATOR . $clean_pid . DIRECTORY_SEPARATOR . basename($file['source']);
  }
  
  return $files_to_add;
}
