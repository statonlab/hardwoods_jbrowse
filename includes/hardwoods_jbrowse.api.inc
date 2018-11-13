<?php

/**
 * Get saved settings.
 *
 * @return null
 */
function hardwoods_jbrowse_get_settings() {
  $default = [
    'bin_path' => '',
    'link' => '',
    'data_dir' => '',
  ];

  $settings = variable_get('hardwoods_jbrowse_settings', json_encode($default));

  return json_decode($settings, TRUE);
}

/**
 * Save settings.
 *
 * @param array $settings
 *
 * @return array The final merged settings
 */
function hardwoods_jbrowse_save_settings($settings) {
  $default = [
    'bin_path' => '',
    'link' => '',
    'data_dir' => '',
  ];

  $final = [];

  foreach ($default as $key => $value) {
    if (isset($settings[$key])) {
      $final[$key] = $settings[$key];
    }
    else {
      $final[$key] = $default[$key];
    }
  }

  variable_set('hardwoods_jbrowse_settings', json_encode($final));

  return $final;
}

/**
 * Get an array to instances.
 *
 * @return mixed
 */
function hardwoods_jbrowse_get_instances($conditions = NULL) {
  static $users = [];
  static $organisms = [];

  $instances = db_select('hardwoods_jbrowse_instances', 'H')->fields('H');

  if ($conditions) {
    foreach ($conditions as $field => $value) {
      $instances->condition($field, $value);
    }
  }

  $instances = $instances->execute()->fetchAll();

  foreach ($instances as $key => &$instance) {
    if (!isset($users[$instance->uid])) {
      $users[$instance->uid] = user_load($instance->uid);
    }
    $instance->user = $users[$instance->uid];

    if (!isset($organisms[$instance->organism_id])) {
      $organisms[$instance->organism_id] = chado_query(
        'SELECT * FROM {organism} WHERE organism_id=:id',
        [':id' => $instance->organism_id]
      )->fetchObject();
    }
    $instance->organism = $organisms[$instance->organism_id];
  }

  return $instances;
}

/**
 * Create a new instance.
 *
 * @param $data
 *
 * @return \DatabaseStatementInterface|int
 * @throws \Exception
 */
function hardwoods_jbrowse_create_instance($data) {
  global $user;

  $instance_id = db_insert('hardwoods_jbrowse_instances')->fields(
    [
      'uid' => $user->uid,
      'organism_id' => $data['organism_id'],
      'title' => $data['title'],
      'description' => isset($data['description']) ? $data['description'] : '',
      'created_at' => $data['created_at'],
      'file' => $data['file'],
    ]
  )->execute();

  if (!$instance_id) {
    return FALSE;
  }

  return $instance_id;
}

/**
 * Get an instance by id.
 *
 * @param $instance_id
 *
 * @return mixed
 */
function hardwoods_jbrowse_get_instance($instance_id) {
  $instance = hardwoods_jbrowse_get_instances(['id' => $instance_id]);

  return reset($instance);
}

/**
 * Update an instance.
 *
 * @param int $id
 * @param array $data
 *
 * @return \DatabaseStatementInterface
 */
function hardwoods_jbrowse_update_instance($id, $data) {
  return db_update('hardwoods_jbrowse_instances')
    ->fields($data)
    ->condition('id', $id)
    ->execute();
}

/**
 * @param int|object $instance
 *
 * @return int
 * @throws \Exception
 */
function hardwoods_jbrowse_delete_instance($instance) {
  if (is_object($instance) && property_exists($instance, 'id')) {
    $id = $instance->id;
  }
  elseif (is_numeric($instance)) {
    $id = $instance;
  }
  else {
    throw new Exception(
      'Unable to extract instance ID. Please provide a valid ID to delete the instance.'
    );
  }

  return db_delete('hardwoods_jbrowse_instances')
    ->condition('id', $id)
    ->execute();
}

/**
 * Create a new JBrowse track for a given instance.
 *
 * @param $data
 *
 * @return bool|int Track ID or FALSE if an error occurs.
 * @throws \Exception
 */
function hardwoods_jbrowse_create_track($instance, $data) {
  global $user;

  $track_id = db_insert('hardwoods_jbrowse_tracks')->fields(
    [
      'uid' => $user->uid,
      'instance_id' => $instance->id,
      'organism_id' => $instance->organism_id,
      'label' => $data['label'],
      'track_type' => $data['track_type'],
      'file_type' => $data['file_type'],
      'created_at' => $data['created_at'],
      'file' => $data['file'],
    ]
  )->execute();

  if (!$track_id) {
    return FALSE;
  }

  return $track_id;
}

/**
 * Delete a track by ID.
 *
 * @param $track_id
 *
 * @return int
 */
function hardwoods_jbrowse_delete_track($track_id) {
  return db_delete('hardwoods_jbrowse_tracks')
    ->condition('id', $track_id)
    ->execute();
}

/**
 * Get attached tracks with users pre-loaded.
 *
 * @param $instance
 *
 * @return mixed
 */
function hardwoods_jbrowse_get_tracks($instance, array $conditions = []) {
  static $users = [];

  $tracks = db_select('hardwoods_jbrowse_tracks', 'HJT')->fields('HJT');

  foreach ($conditions as $field => $value) {
    $tracks->condition($field, $value);
  }

  $tracks = $tracks->condition('instance_id', $instance->id)
    ->execute()
    ->fetchAll();

  foreach ($tracks as &$track) {
    if (!isset($users[$track->uid])) {
      $users[$track->uid] = user_load($track->uid);
    }

    $track->user = $users[$track->uid];
  }

  return $tracks;
}

/**
 * Get a track with instance and user data attached.
 *
 * @param $track_id
 *
 * @return mixed
 */
function hardwoods_jbrowse_get_track($track_id) {
  $track = db_select('hardwoods_jbrowse_tracks', 'HJT')
    ->fields('HJT')
    ->condition('id', $track_id)
    ->execute()
    ->fetchObject();

  $track->user = user_load($track->uid);
  $track->instance = hardwoods_jbrowse_get_instance($track->instance_id);

  return $track;
}

/**
 * @param $track
 * @param array $fields
 *
 * @return \DatabaseStatementInterface
 */
function hardwoods_jbrowse_update_track($track, array $fields) {
  return db_update('hardwoods_jbrowse_tracks')
    ->fields($fields)
    ->condition('id', $track->id)
    ->execute();
}

/**
 * Get a list of organisms.
 *
 * @return mixed
 */
function hardwoods_jbrowse_get_organisms_list() {
  return db_select('chado.organism', 'CO')
    ->fields('CO', ['organism_id', 'genus', 'species', 'common_name'])
    ->execute()
    ->fetchAll();
}

function hardwoods_jbrowse_construct_organism_name($organism) {
  $name = $organism->genus;
  $name .= " $organism->species";

  if (!empty($organism->common_name)) {
    $name .= " ($organism->common_name)";
  }

  return $name;
}

function hardwoods_jbrowse_make_slug($string) {
  $slug = str_replace(' ', '_', $string);
  $slug = str_replace('(', '_', $slug);
  $slug = str_replace(')', '_', $slug);
  $slug = str_replace('[', '_', $slug);
  $slug = str_replace(']', '_', $slug);
  $slug = str_replace('!', '_', $slug);
  $slug = str_replace('?', '_', $slug);
  $slug = str_replace('"', '_', $slug);
  $slug = str_replace('\'', '_', $slug);
  $slug = str_replace('\\', '_', $slug);
  $slug = str_replace(':', '_', $slug);

  return strtolower(trim($slug, '_'));
}

function hardwoods_jbrowse_upload_file($field) {
  $file = file_save_upload($field, [
      'file_validate_extensions' => ['fasta faa fna fastq txt gff vcf wig gz tbi bw'],
      'file_validate_size' => [1024 * 1024 * 1024 * 20] // Make it 20 GB max
    ]
  );

  return !$file ? FALSE : $file; // drupal_realpath($file->uri);
}

/**
 * Moves a file to an intermediate directory, then to the destination, if given.
 *
 * @param $file
 *  The file object.
 *
 * @param $path
 *  The path to the directory of the new object.
 *  If the directory provided does not exist, it will be created.
 *
 * @return
 *  The moved file.
 */
function hardwoods_jbrowse_move_file($file, $path = NULL) {
  $directory = 'public://tripal/hardwoods_jbrowse';
  file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
  $file = file_move($file, $directory);

  if (isset($path)) {
    file_prepare_directory($path, FILE_CREATE_DIRECTORY);
    $oldname = drupal_realpath($directory) . '/' . $file->filename;
    $newname = $path . '/' . $file->filename;
    rename($oldname, $newname);
  }

  return $file;
}

/**
 * Copy a file into a new directory.
 * If the directory provided does not exist, it will be created.
 *
 * @param $source
 *  File path of the source file.
 * @param $destination
 *  File path to the destination directory.
 *
 * @return bool
 */
function hardwoods_jbrowse_copy_file($source, $destination)
{
  file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

  $file_name = pathinfo($source)['basename'];
  $destination = $destination . '/' . $file_name;

  return copy($source, $destination);
}


/**
 * Build the http query for a given instance to link to JBrowse.
 *
 * @param $instance
 *
 * @return array
 */
function hardwoods_jbrowse_build_http_query($instance) {
  $path = hardwoods_jbrowse_make_slug($instance->title);
  $tracks = hardwoods_jbrowse_get_tracks($instance);
  $tracks_path = '';
  if (!empty($tracks)) {
    $tracks_path = implode(
      ',',
      array_map(
        function ($track) {
          return hardwoods_jbrowse_make_slug($track->label);
        },
        $tracks
      )
    );
  }

  return [
    'data' => "data/$path/data",
    'tracks' => $tracks_path,
  ];
}

/**
 * Get trackList.json for an instance.
 *
 * @param $instance
 *
 * @return array Decoded json array.
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_get_json($instance) {
  $path = hardwoods_jbrowse_get_track_list_file_path($instance);

  $contents = file_get_contents($path);
  if (!$contents) {
    throw new Exception('Unable to find ' . $path . ' file');
  }

  return json_decode($contents, TRUE);
}

/**
 * @param $instance
 * @param $data
 *
 * @throws \Exception
 * @return bool|int
 */
function hardwoods_jbrowse_save_json($instance, $data) {
  $path = hardwoods_jbrowse_get_track_list_file_path($instance);

  $default = hardwoods_jbrowse_get_json($instance);
  $json = $data + $default;
  $encoded = json_encode($json, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  return file_put_contents($path, $encoded);
}

/**
 * @param $instance
 *
 * @return string
 */
function hardwoods_jbrowse_get_track_list_file_path($instance) {
  $settings = hardwoods_jbrowse_get_settings();

  $path = $settings['data_dir'];
  $path .= '/' . hardwoods_jbrowse_make_slug($instance->title);
  $path .= '/data/trackList.json';

  return $path;
}

/**
 * Gets a list of supported track types.
 *
 * @return array
 */
function hardwoods_get_track_types() {
  return [
    'FeatureTrack',
    'CanvasFeatures',
    'HTMLFeatures',
    'HTMLVariants',
    'XYPlot',
  ];
}
