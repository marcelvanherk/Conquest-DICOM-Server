<template>
  <v-dialog :value="dWait" hide-overlay persistent width="300">
    <v-card color="primary" dark>
      <v-card-text>
        Aguarde por favor...
        <v-progress-linear
          indeterminate
          color="white"
          class="mb-0"
        ></v-progress-linear>
      </v-card-text>
    </v-card>
  </v-dialog>
</template>

<script>
export default {
  name: 'WaitDialog',
  props: {
    dWait: { type: Boolean },
  },
}
</script>
