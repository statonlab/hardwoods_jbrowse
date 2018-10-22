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

  $settings = hardwoods_jbrowse_get_settings();

  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];

  $path = $data . '/' . hardwoods_jbrowse_make_slug($instance->title);
  if (!file_exists($path)) {
    if (!mkdir($path)) {
      throw new Exception(
        'Unable to make data directory! Please make sure the directory 
      at ' . $data . ' exists and is writable by the current user.'
      );
    }
  }

  var_dump($instance->file);

  $out = $path . '/data';

  $ret = hardwoods_jbrowse_safe_exec(
    'perl',
    [
      $bin . '/prepare-refseqs.pl',
      '--fasta',
      $instance->file,
      '--out',
      $out,
    ]
  );
}
