<?php

/**
 * @param $instance_id
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_create_instance_files($instance_id) {
  $instance = hardwoods_jbrowse_get_instance($instance_id);

  if (empty($instance)) {
    throw new Exception('Unable to find instance to create files for.');
  }

  hardwoods_jbrowse_cmd_prepare_refseq($instance);
}


/**
 * @param $track_id
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_add_track_to_instance($track_id) {
  $track = hardwoods_jbrowse_get_track($track_id);

  if (empty($track)) {
    throw new Exception('Unable to find instance to create files for.');
  }

  hardwoods_jbrowse_cmd_add_track($track);
}
