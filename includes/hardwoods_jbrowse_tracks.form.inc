<?php

/**
 * Add a track to an instance form.
 *
 * @param $form
 * @param $form_state
 * @param $instance_id
 *
 * @return array
 */
function hardwoods_jbrowse_add_track_form($form, &$form_state, $instance_id) {
  if (empty(hardwoods_jbrowse_get_instance($instance_id))) {
    drupal_not_found();
    return [];
  }

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Track Label'),
    '#description' => t('This will appear on the sidebar.'),
    '#required' => TRUE,
  ];

  $form['instance_id'] = [
    '#type' => 'hidden',
    '#value' => $instance_id,
  ];

  $form['data'] = [
    '#type' => 'fieldset',
    '#title' => t('Track Files'),
  ];

  $form['data']['track_type'] = [
    '#type' => 'select',
    '#description' => t('See http://gmod.org/wiki/JBrowse_Configuration_Guide#flatfile-to-json.pl for more info.'),
    '#required' => TRUE,
    '#title' => t('Track Type'),
    '#options' => drupal_map_assoc(hardwoods_get_track_types()),
  ];

  $form['data']['file_type'] = [
    '#type' => 'select',
    '#title' => t('File Type'),
    '#options' => drupal_map_assoc(['gff', 'bed', 'gbk', 'vcf', 'bw']),
    '#description' => t(
      'See http://gmod.org/wiki/JBrowse_Configuration_Guide#flatfile-to-json.pl for more info.'
    ),
    '#required' => TRUE,
  ];

  $form['data']['file'] = [
    '#type' => 'file',
    '#title' => t('Reference Sequence FASTA File'),
  ];

  $form['data']['file_path'] = [
    '#type' => 'textfield',
    '#title' => t('- OR Path to File on Server -'),
    '#description' => t('This path will be ignored if a file is provided above. Ex: sites/default/files/file.fasta or /data/file.fasta'),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Add New Track',
  ];

  return $form;
}

/**
 * Validate the add track form.
 *
 * @param array $form
 * @param array $form_state
 */
function hardwoods_jbrowse_add_track_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $file = $_FILES['files']['tmp_name']['file'];
  $settings = hardwoods_jbrowse_get_settings();
  $instance = hardwoods_jbrowse_get_instance($values['instance_id']);
  $data = $settings['data_dir'];
  $extension = $values['file_type'];
  $path = NULL;

  switch ($extension)
  {
    case 'vcf':
      $path = $data . '/' . hardwoods_jbrowse_make_slug($instance->title) . '/data/vcf';
      break;
    case 'bw':
      $path = $data . '/' . hardwoods_jbrowse_make_slug($instance->title) . '/data/wig';
      break;
  }

  $local_file = isset($values['file_path']) ? $values['file_path'] : NULL;

  if (empty($file) && empty($local_file)) {
    form_set_error('file',
      'Please provide a local file path or upload a new file.');
  }
  elseif (empty($file) && !empty($local_file)) {
    if (!file_exists($local_file)) {
      form_set_error('file_path', 'The file path provided does not exist.');
    }
    else {
      hardwoods_jbrowse_copy_file($local_file, $path);
    }
  }
  else {
    $uploaded = hardwoods_jbrowse_upload_file('file');
    if (!$uploaded) {
      form_set_error('file', 'Unable to upload file');
    }
    else {
      $form_state['values']['uploaded_file'] = $file;
      hardwoods_jbrowse_move_file($uploaded, $path);
    }
  }
}

/**
 * Handle form submission for adding a track.
 *
 * @param array $form
 * @param array $form_state
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_add_track_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];

  $file = isset($values['file_path']) ? $values['file_path'] : NULL;
  if (!empty($values['uploaded_file'])) {
    $file = $values['uploaded_file'];
  }

  $instance = hardwoods_jbrowse_get_instance($values['instance_id']);

  $track_id = hardwoods_jbrowse_create_track($instance, [
    'label' => $values['label'],
    'track_type' => $values['track_type'],
    'file_type' => $values['file_type'],
    'file' => $file,
    'created_at' => time(),
  ]);

  tripal_add_job('Add JBrowse track to ' . $instance->title,
    'hardwoods_jbrowse', 'hardwoods_jbrowse_add_track_to_instance', [$track_id],
    $user->uid);

  drupal_goto('admin/hardwoods_jbrowse/instances/' . $instance->id);
}

/**
 * Delete a track form.
 *
 * @param array $form
 * @param array $form_state
 * @param int $track_id
 *
 * @return array
 */
function hardwoods_jbrowse_delete_track_form($form, &$form_state, $track_id) {

  $track = hardwoods_jbrowse_get_track($track_id);

  if (!$track->id) {
    $form['error'] = [
      '#type' => 'item',
      '#markup' => '<p style="color: red">Unable to find track.</p>',
    ];
    return $form;
  }

  $form['description'] = [
    '#type' => 'item',
    '#markup' => 'Are you sure you want to delete the ' . $track->label . ' track?',
  ];

  $form['track_id'] = [
    '#type' => 'hidden',
    '#value' => $track_id,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Delete Track',
  ];

  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l('Cancel',
      'admin/hardwoods_jbrowse/instances/' . $track->instance_id),
  ];

  return $form;
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_delete_track_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];

  $track = hardwoods_jbrowse_get_track($values['track_id']);

  tripal_add_job('Delete JBrowse track', 'hardwoods_jbrowse',
    'hardwoods_jbrowse_delete_track_from_instance', [$values['track_id']],
    $user->uid);

  hardwoods_jbrowse_update_track($track, ['is_deleted' => 1]);

  drupal_goto('admin/hardwoods_jbrowse/instances/' . $track->instance_id);
}
