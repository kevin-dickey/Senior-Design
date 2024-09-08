import React from 'react';

import {Formik, Form, Field} from 'formik';
import * as Yup from 'yup';

import {Effect} from '../serialization/Effect';

import {Box, Button} from "@mui/material";
import Grid from "@mui/material/Grid";

import {Delete, Save} from '@mui/icons-material';

interface EffectFormProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
    children?: React.ReactNode;
}

export const EffectSchema = Yup.object().shape({
    name: Yup
        .string()
        .required('Required'),
    startTimeMs: Yup
        .number()
        .integer('Must be an integer')
        .positive()
        .min(0, 'Must be greater than or equal to 0')
        .required('Required'),
    durationMs: Yup
        .number()
        .integer('Must be an integer')
        .positive()
        .min(1, 'Must be greater than 0')
        .required('Required')
});

export const EffectForm: React.FC<EffectFormProps> = (
    {effect, onSubmit, onDelete, children}
) => {


    return (
        <Box
            className="effect-form"
            sx={{
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'left',
                justifyContent: 'left',
                textAlign: 'left'
            }}
        >
            <Formik
                initialValues={effect}
                validationSchema={EffectSchema}
                onSubmit={(values) => {
                    onSubmit(values);
                }}
            >
                {({values, errors, touched}) => (
                    <Form>
                        <Grid container spacing={2}>
                            <Grid item xs={12}>
                                <h3>Effect - {values.name}</h3>
                            </Grid>
                            <Grid item xs={4}>
                                <label htmlFor="name">Name</label>
                            </Grid>
                            <Grid item xs={8}>
                                {errors.name && touched.name ? (
                                    <div>{errors.name}</div>
                                ) : null}
                                <Field
                                    id="name"
                                    name="name"
                                    placeholder="Effect Name"
                                    type="text"
                                />
                            </Grid>
                            <Grid item xs={4}>
                                <label htmlFor="start">Start</label>
                            </Grid>
                            <Grid item xs={8}>
                                {errors.startTimeMs && touched.startTimeMs ? (
                                    <div>{errors.startTimeMs}</div>
                                ) : null}
                                <Field id="startTimeMs" name="startTimeMs" type="number"/>
                            </Grid>
                            <Grid item xs={4}>
                                <label htmlFor="duration">Duration</label>
                            </Grid>
                            <Grid item xs={8}>
                                {errors.durationMs && touched.durationMs ? (
                                    <div>{errors.durationMs}</div>
                                ) : null}
                                <Field id="durationMs" name="durationMs" type="number"/>
                            </Grid>
                            {children}
                            {onDelete && (
                                <Button
                                    type="button"
                                    variant="outlined"
                                    startIcon={<Delete />}
                                    onClick={() => {
                                        onDelete(effect.id);
                                    }}
                                >
                                    Delete
                                </Button>
                            )}
                            <Button
                                type="submit"
                                variant="contained"
                                endIcon={<Save />}
                            >
                                Save
                            </Button>
                        </Grid>
                    </Form>
                )}
            </Formik>
        </Box>
    );
};
