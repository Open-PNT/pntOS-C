#pragma once

#include <pntos/plugins/common.h>
#include <pntos/plugins/state_modeling.h>

#ifdef __cplusplus
extern "C" {
#endif

PNTOS_ASSUME_NONNULL_BEGIN

/**
 * A strategy capable of Bayesian inference on a linearized discrete-time system with Gaussian noise
 * inputs. This struct serves as a base class for all fusion strategies. The PntosFusionType may be
 * used to determine the type of fusion that this struct supports, and thereby which implementation
 * of fusion strategy this struct is castable to.
 *
 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS API.
 * Usage of this feature is highly discouraged in non-experimental code, and its definition may
 * change at any time.
 */
typedef struct PntosCommonFusionStrategy {
	PntosManagedMemory* memory;

	/**
	 * The type of fusion model used by this strategy
	 */
	PntosFusionType engine_type;

} PntosCommonFusionStrategy;

/**
 * Assumes the system is described by discrete-time matrices and noise inputs are zero-mean white
 * Gaussian.
 *
 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS API.
 * Usage of this feature is highly discouraged in non-experimental code, and its definition may
 * change at any time.
 */
typedef struct PntosStandardFusionStrategy {
	PntosCommonFusionStrategy common;

	/**
	 * Return the total number of states added to this filter.
	 */
	size_t (*get_num_states)(struct PntosStandardFusionStrategy* self);

	/**
	 * Increase number of filter states and set the estimate, covariance, and cross-covariance
	 * (optional).  Returns index of the first added state. If \p cross_covariance is NULL, cross
	 * covariance between the existing states and the added states will be set to zeroes.
	 */
	size_t (*add_states)(struct PntosStandardFusionStrategy* self,
	                     PntosMatrix* initial_estimate,
	                     PntosMatrix* initial_covariance,
	                     PntosMatrix* PNTOS_NULLABLE cross_covariance);

	/**
	 * Decrease the number of states by \p count, starting at \p first_index. \p first_index plus \p
	 * count cannot be greater than the value returned by #get_num_states.
	 */
	void (*remove_states)(struct PntosStandardFusionStrategy* self,
	                      size_t first_index,
	                      size_t count);

	/**
	 * Get the current state estimate. Returns NULL if no states have been added to this fusion
	 * strategy. Guaranteed to not be NULL if #get_num_states returns a value other than 0.
	 */
	PntosMatrix* PNTOS_NULLABLE (*get_estimate)(struct PntosStandardFusionStrategy* self);

	/**
	 * Set a slice of the state vector. \p first_index plus the number of rows in \p new_estimate
	 * must be less than or equal to the value returned by #get_num_states. The number of columns in
	 * \p new_estimate must be equal to 1.
	 */
	void (*set_estimate_slice)(struct PntosStandardFusionStrategy* self,
	                           PntosMatrix* new_estimate,
	                           size_t first_index);

	/**
	 * Get the current state covariance. Returns NULL if no states have been added to this fusion
	 * strategy. Guaranteed to not be NULL if #get_num_states returns a value other than 0.
	 */
	PntosMatrix* PNTOS_NULLABLE (*get_covariance)(struct PntosStandardFusionStrategy* self);

	/**
	 * Set a slice of the state covariance by first row and column. \p first_row plus the number of
	 * rows in \p new_covariance must be less than or equal to the value returned by
	 * #get_num_states. \p first_col plus the number of columns in \p new_covariance must be less
	 * than or equal to the value returned by #get_num_states.
	 */
	void (*set_covariance_block)(struct PntosStandardFusionStrategy* self,
	                             PntosMatrix* new_covariance,
	                             size_t first_row,
	                             size_t first_col);

	/**
	 * Set a slice of the state covariance along the diagonal. \p first_state plus the number of
	 * rows in \p new_covariance must be less than or equal to the value returned by
	 * #get_num_states. \p first_state plus the number of columns in \p new_covariance must be less
	 * than or equal to the value returned by #get_num_states.
	 */
	void (*set_covariance_slice)(struct PntosStandardFusionStrategy* self,
	                             PntosMatrix* new_covariance,
	                             size_t first_state);

	/**
	 * Propagates the state estimate and covariance forward one time epoch using the given dynamics
	 * model. #get_num_states must return a value greater than zero before this method can be
	 * called. Also, the Phi matrix in \p dynamics_model must have a number of columns equal to the
	 * value returned by #get_num_states.
	 */
	void (*propagate)(struct PntosStandardFusionStrategy* self,
	                  PntosStandardDynamicsModel* dynamics_model);

	/**
	 * Updates the state estimate and covariance at the current time using the given measurement
	 * model. #get_num_states must return a value greater than zero before this method can be
	 * called. Also, the H matrix in \p measurement_model must have a number of columns equal to
	 * the value returned by #get_num_states.
	 */
	void (*update)(struct PntosStandardFusionStrategy* self,
	               PntosStandardMeasurementModel* measurement_model);

	/**
	 * Produce a deep-copy this fusion strategy instance.
	 */
	struct PntosStandardFusionStrategy* (*clone)(struct PntosStandardFusionStrategy* self);
} PntosStandardFusionStrategy;

/**
 * Plugin that provides a PntosCommonFusionStrategy, allowing swapping of underlying algorithms for
 * filtering.
 *
 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS API.
 * Usage of this feature is highly discouraged in non-experimental code, and its definition may
 * change at any time.
 */
typedef struct PntosFusionStrategyPlugin {
	PntosCommonPlugin common;

	/**
	 * Return if the plugin supports a given type of fusion. See #PntosFusionType.
	 */
	bool (*is_fusion_type_supported)(struct PntosFusionStrategyPlugin* self, PntosFusionType type);

	/**
	 * Create an instance of #PntosCommonFusionStrategy. The #PntosFusionType parameter specifies
	 * the type of fusion that the returned value will support. For example, if the user passes in
	 * #PNTOS_FUSION_STANDARD_MODEL, then the returned value will be castable to
	 * PntosStandardFusionStrategy. Returns NULL if \p type is not supported by this fusion strategy
	 * plugin. Guaranteed to not return NULL if #is_fusion_type_supported returns true for \p type.
	 */
	PntosCommonFusionStrategy* PNTOS_NULLABLE (*new_fusion_strategy)(
	    struct PntosFusionStrategyPlugin* self, PntosFusionType type);
} PntosFusionStrategyPlugin;

PNTOS_ASSUME_NONNULL_END

#ifdef __cplusplus
}
#endif
