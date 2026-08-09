#pragma once

#include <pntos/annotations.h>
#include <pntos/memory.h>
#include <pntos/stdbool.h>

#include <pntos/stdint.h>
#include <stddef.h>

#include <pntos/aspn.h>
#include <pntos/type_enum.h>

#ifdef __cplusplus
extern "C" {
#endif

PNTOS_ASSUME_NONNULL_BEGIN

/**
 * The current API version for the plugin declarations. Any time a breaking change is made to the
 * plugin APIs, this number will be incremented before the next release. Code which is using a pntOS
 * plugin can inspect the PntosPluginsDescription.loader_api_version field to see which API version
 * the compiled code was using.
 *
 * A breaking change is defined as: any change to the API that causes plugins written against and
 * conforming to the rules of the previous version of the API to no longer conform to the rules of
 * the new version of the API, when the plugin is recompiled against the new API headers.
 *
 * This version macro only applies to released versions of pntOS.
 */
#define PNTOS_PLUGIN_API_VERSION 2

/**
 * A container for an ASPN message. This container may contain either proper ASPN messages which are
 * part of the ASPN data model, or extension messages specific to pntOS which augment ASPN. For
 * messages of the former type, the wrapped message's message_type field should be used directly.
 * For messages of the latter type, cast the wrapped message's message_type field to
 * PntosMessageType.
 *
 * Invariants:
 * - PntosMessage.source_identifier must be a pointer to an array of allocated memory ending in a
 * `\0` character
 */
typedef struct PntosMessage {
	PntosManagedMemory* memory;
	/**
	 * Either an ASPN message or a pntOS ASPN Extension message, depending on the value of
	 * wrapped_message.message_type.
	 */
	AspnBase* wrapped_message;
	/**
	 * A `\0` terminated C string. The string represents a source identifier indicating where this
	 * message came from. If the message originated from a transport plugin and the underlying
	 * transport has the concept of a channel or topic, this field should be populated by the
	 * channel or topic. Otherwise, the identifier is populated in a plugin-specific manner by the
	 * originating plugin that created the message.
	 */
	char* source_identifier;
} PntosMessage;

/**
 * An owned array of PntosMessage.
 */
typedef struct PntosMessageArray {

	PntosManagedMemory* memory;

	/**
	 * An array of length #num_messages which contains nullable pointers to PntosMessage.
	 */
	PntosMessage* PNTOS_NULLABLE* data;

	/**
	 * The number of PntosMessage instances stored in #data (including NULL instances). For example,
	 * `data[num_messages-1]` points to the last message stored in this structure.
	 */
	size_t num_messages;

} PntosMessageArray;

/**
 * An owned C string (see PntosManagedMemory for more information).
 *
 * Invariants:
 * - data must be a pointer to an array of allocated memory ending in a `\0` character
 */
typedef struct PntosString {
	PntosManagedMemory* memory;
	/**
	 * A `\0` terminated C string.
	 */
	char* data;
} PntosString;

/**
 * An owned array of bytes (see PntosManagedMemory for more information).
 *
 * Invariants:
 * - data must be a pointer to an array of allocated memory
 * - The length of data must be equal to `sizeof(unsigned char)*num_bytes`
 */
typedef struct PntosByteArray {
	PntosManagedMemory* memory;
	/**
	 * An array of #num_bytes bytes which is not `\0` terminated.  `\0` characters may be
	 * present in the data stream.
	 */
	unsigned char* data;

	/**
	 * The length of #data.
	 */
	size_t num_bytes;
} PntosByteArray;

/**
 * An owned array of strings (see PntosManagedMemory for more information).
 *
 * Invariants:
 * - Each member of the data array must be a pointer to an array of allocated memory ending in a
 *   `\0` character
 * - The length of data must be equal to `sizeof(char*)*num_strings`
 */
typedef struct PntosStringArray {
	PntosManagedMemory* memory;
	/**
	 * An array of #num_strings pointers to `\0` terminated C strings.
	 */
	char** data;

	/**
	 * Number of `\0` terminated C strings in #data.  For example, `data[num_strings-1]` points
	 * to the last C string in this structure.
	 */
	size_t num_strings;
} PntosStringArray;
/**
 * An owned array of integers (see PntosManagedMemory for more information).
 *
 * Invariants:
 * - The length of data must be `sizeof(int64_t)*num_ints`
 */
typedef struct PntosIntArray {
	PntosManagedMemory* memory;
	/**
	 * An array of #num_ints ints.
	 */
	int64_t* data;
	/**
	 * The number of ints stored in #data. For example, `data[num_ints-1]` points to the last int
	 * stored in this structure.
	 */
	size_t num_ints;

} PntosIntArray;
/**
 * An owned array of doubles (see PntosManagedMemory for more information).
 *
 * Invariants:
 * - The length of data must be `sizeof(double)*num_doubles`
 */
typedef struct PntosDoubleArray {
	PntosManagedMemory* memory;
	/**
	 * An array of #num_doubles doubles.
	 */
	double* data;
	/**
	 * The number of doubles stored in #data. For example, `data[num_doubles-1]` points to the last
	 * double stored in this structure.
	 */
	size_t num_doubles;

} PntosDoubleArray;

/**
 * A 2D container (row, col) full of doubles.
 *
 * Invariants:
 * - The length of data must be `sizeof(double)*rows*cols`
 */
typedef struct PntosMatrix {
	PntosManagedMemory* memory;
	/**
	 * The number of rows in this matrix. For a MxN matrix, `rows` is M.
	 */
	size_t rows;
	/**
	 * The number of columns in this matrix. For a MxN matrix, `cols` is N.
	 */
	size_t cols;
	/**
	 * A set of `rows` x `cols` doubles. Data is stored in row major form.
	 */
	double* data;
} PntosMatrix;

/**
 * Describes how the fields in PntosEstimateWithCovariance are used.
 */
typedef enum PntosEstimateWithCovarianceType {

	/**
	 * Contains a mean (estimate) and covariance describing a multivariate Gaussian distribution.
	 *  - PntosEstimateWithCovariance.length reflects the size of the estimate and covariance
	 *    fields.
	 *  - PntosEstimateWithCovariance.estimate is size Nx1 where N is the length field.
	 *  - PntosEstimateWithCovariance.covariance is size NxN where N is the length field.
	 */
	PNTOS_EWC_GENERIC,
	/**
	 * Contains a mean (estimate) and covariance describing a rotation modeled by a multivariate
	 * Gaussian distribution, but the estimate is in quaternion form and the covariance is in tilt
	 * error form.
	 *  - PntosEstimateWithCovariance.length is unused.
	 *  - PntosEstimateWithCovariance.estimate is size 4x1
	 *  - PntosEstimateWithCovariance.covariance is size 3x3, in radians^2.
	 */
	PNTOS_EWC_ATTITUDE_QUAT
} PntosEstimateWithCovarianceType;

/**
 * A container for holding an estimate and covariance.
 */
typedef struct PntosEstimateWithCovariance {
	PntosManagedMemory* memory;

	/**
	 * Describes how the fields in this struct are used.
	 */
	PntosEstimateWithCovarianceType type;

	/**
	 * Usage depends on the #type field.
	 */
	size_t length;

	/**
	 * An array of doubles representing an estimate vector.  Usage depends on the #type field.
	 */
	double* estimate;

	/**
	 * An array of doubles representing a square covariance matrix.  Data is stored in row major
	 * form.  Usage depends on the #type field.
	 */
	double* covariance;
} PntosEstimateWithCovariance;

/**
 * An owned array of PntosMessageType.
 */
typedef struct PntosMessageTypeArray {
	PntosManagedMemory* memory;
	/**
	 * An array of #num_types types.
	 */
	PntosMessageType* data;
	/**
	 * The number of types stored in #data. For example, `data[num_types - 1]` points to the last
	 * type stored in this structure.
	 */
	size_t num_types;
} PntosMessageTypeArray;

/**
 * An enumeration of the types of plugins supported by pntOS for this loader API version. Each enum
 * entry maps to a corresponding structure with PascalCase naming. For example, the
 * #PNTOS_CONTROLLER_PLUGIN value in this enum is indicating a plugin represented by the struct
 * `PntosControllerPlugin`. Note that because the utility plugin has no additional API requirements
 * beyond the PntosCommonPlugin, there is not a `PntosUtilityPlugin`. Instead, implementers of
 * #PNTOS_UTILITY_PLUGIN should implement and return a PntosCommonPlugin.
 */
typedef enum PntosPluginTypes {
	/**
	 * A unused entry, designed to allow code to detect accidentally unset fields. This value must
	 * not be used by any plugin implementation, other than to check for an erroneous default value
	 * being used.
	 */
	PNTOS_UNDEFINED_PLUGIN,
	/**
	 * The primary plugin that controls the entire operation of pntOS. After the pntOS loader
	 * collects the set of plugins available, execution passes to the controller plugin. At that
	 * point, the controller is responsible for deciding how it should use, when it should use, and
	 * if it should use the other plugins, including routing communications between them and
	 * controlling the concurrency model used.
	 *
	 * The controller plugin is designed to work together with the platform integration plugin (PIP)
	 * to control overall behavior of the system. In particular, the controller consists of
	 * re-useable logic that is not specific to a particular solution, whereas the PIP consists of
	 * platform-specific logic. The primary job of the controller plugin is to place each plugin
	 * into a concurrency primitive (thread, process, etc.) and then manage the communications
	 * between those plugins via the mediator the controller provides. The controller may also
	 * configure a single threaded approach if desired.
	 *
	 * Any logic which is specific to a particular solution, platform, or vehicle should be placed
	 * in the platform integration plugin instead of the controller. After the controller is done
	 * initially setting up the concurrency model, it should hand control to the PIP, to handle
	 * command and control of the plugins outside the scope of inter-plugin communications and
	 * message routing between plugins.
	 */
	PNTOS_CONTROLLER_PLUGIN,
	/**
	 * A plugin that models an information fusion approach. This plugin accepts modular
	 * representations of state space models, sensor descriptions, raw measurements, and
	 * computational filtering engines, all provided by external sources. It then performs the
	 * bookkeeping needed to hook these modules up to each other in a extensible and flexible way.
	 * State space models and sensor descriptions are provided by state modeling plugins, raw
	 * measurements are provided by transport plugins, and filtering engines are provided by fusion
	 * strategy plugins.
	 */
	PNTOS_FUSION_PLUGIN,
	/**
	 * A low level computational engine that can perform sensor fusion given pre-determined fixed
	 * models of errors and raw measurements. Because this plugin requires fixed models, another
	 * plugin is required to orchestrate modular descriptions of the fusion problem into a
	 * fixed-size problem. The fusion plugin is used to orchestrate modular descriptions into a
	 * fixed-size problem suitable for the computational engine in this plugin to consume.
	 */
	PNTOS_FUSION_STRATEGY_PLUGIN,
	/**
	 * An output plugin for pntOS to interact with the platform it is running on. While pntOS uses a
	 * uniform set of conventions internally (e.g. ASPN messages), pntOS is often used on legacy
	 * platforms that are non-cooperative and impose requirements on the input/output expected of
	 * pntOS. For example, analog time outputs or PVA messages at a certain rate and non-ASPN format
	 * might be required by a vehicle that pntOS is running on. The platform integration plugin
	 * converts pntOS internal messages into whatever external outputs may be needed on the current
	 * platform. It is designed to be the piece of NRE that must be rewritten when using pntOS on a
	 * novel platform.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_PLATFORM_INTEGRATION_PLUGIN,
	/**
	 * A plugin that provides initialization algorithms. In general, this plugin must be able to
	 * consume some set of measurements and produce an initial solution for the navigation system.
	 * This plugin encompasses everything from a traditional gyrocompass to a cold-start dynamic
	 * positioning.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_INITIALIZATION_PLUGIN,
	/**
	 * A plugin for storing generic datasets that might be consumed by many plugins. For example,
	 * DTED elevation data may be used by many different plugins. Thus, a user might write a
	 * database plugin that encapsulated tiles of elevation data which is available for query by any
	 * other plugin.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_DATABASE_PLUGIN,
	/**
	 * A plugin that listens for incoming sensor/other data on a network bus and provides this data
	 * to pntOS. pntOS has an internally consistent messaging structure which may not match the
	 * messages transmitted over the wire to pntOS. Thus, one of the transport plugin's primary
	 * roles is to convert arbitrarily formatted data off the wire into the internal pntOS ASPN
	 * representation. For network buses that already transmit ASPN data, the transport plugin may
	 * end up being a trivial plugin, simply marshalling data off of a network connection into the
	 * system.
	 */
	PNTOS_TRANSPORT_PLUGIN,
	/**
	 * A plugin for enabling user interfaces to be hooked up to pntOS. This plugin is designed to
	 * enable displays to users to both see the current state of pntOS and also configure/tweak it.
	 * Note that it is *not* designed for hooking up operational displays, but rather debugging /
	 * developer consoles. For outputs that will be sent to operational live displays on the
	 * platform, the platform integration plugin is preferred.
	 */
	PNTOS_UI_PLUGIN,
	/**
	 * A plugin that implements the orchestration monitoring framework. In general,
	 * complementary navigation techniques incur a large risk of solution corruption if any one of
	 * the new sensors is misconfigured, mismodeled, miscalibrated, or otherwise failing. The
	 * orchestration plugin monitors all sensors in the system and generates different
	 * orchestration solutions depending on the user's risk tolerance. Because there is a
	 * huge variety of orchestration approaches, the orchestration plugin is
	 * further modularized into orchestration strategy plugins, which implement a
	 * particular orchestration approach for a particular sensor or situation.
	 */
	PNTOS_ORCHESTRATION_PLUGIN,
	/**
	 * A fine-grained integrity plugin that itself plugins into the larger orchestration
	 * plugin. This plugin is designed to be implementable by a developer who is an expert on a
	 * particular sensor or phenomenology, without being an expert in the entire
	 * orchestration framework used by pntOS. A integrity algorithm specific to a single
	 * sensor or situation may be implemented in this plugin, and then injected into the larger
	 * orchestration which captures many orchestration strategy plugins for
	 * different sensors.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_ORCHESTRATION_STRATEGY_PLUGIN,
	/**
	 * A registry of configuration and status data for pntOS that is available to all plugins.
	 * Registries allow for plugins to have side-channel information shared between plugins without
	 * pntOS being pre-aware of the data that needs to be transmitted. For example, support a plugin
	 * modeling a vision nav sensor has computed a camera calibration matrix. Simultaneously, a UI
	 * plugin would like to show the current calibration matrix to the user, and allow that user to
	 * modify the current matrix if the user desires. Because the vision nav sensor ordinarily would
	 * be modeled by a state modeling plugin and the user-facing UI implemented by a UI plugin,
	 * these two plugins would have no way to communicate current/updated values of this matrix. Via
	 * the registry, these plugins can decide by convention on a key-value that store the
	 * calibration matrix and enable high-speed bi-directional communications of the value of the
	 * calibration matrix in a thread-safe way.
	 *
	 * The registry supports value observers for listeners to be notified when values are changed,
	 * monitoring/logging of when values are changed and by what source, and access control lists to
	 * guard certain keys.
	 */
	PNTOS_REGISTRY_PLUGIN,
	/**
	 * A plugin that generates PVA solutions from an inertial.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_INERTIAL_PLUGIN,
	/**
	 * A plugin that models the errors of the various sensors and systems that measuring the world.
	 * Abstractly, a state modeling plugin includes modular representations of (a) state space
	 * models (including nuisance parameters) and (b) state model providers and how measurements are
	 * related to the states in the state space. Due to there being several mathematical models of
	 * varying fidelity to describe sensors, each state modeling plugin must declare what model it
	 * is using to represent sensor errors. For the standard model, each state modeling plugin is a
	 * bundle of zero or more state blocks, zero or more measurement processors, and zero or more
	 * virtual state blocks.
	 */
	PNTOS_STATE_MODELING_PLUGIN,
	/**
	 * A plugin that logs system events to an arbitrary sink. A sink may be a file, a console, an
	 * attached GUI, a network destination, or any other destination of interest.
	 */
	PNTOS_LOGGING_PLUGIN,
	/**
	 * A utility plugin performs implementation-specific functions that may require access to pntOS
	 * resources (such as the registry) via the ``PntosCommonPlugin`` API. Otherwise, this plugin
	 * has no other API-defined functionality.
	 */
	PNTOS_UTILITY_PLUGIN,
	/**
	 * A plugin that processes data received from a transport before it is sent onward into other
	 * pntOS plugins. Intended use cases include:
	 *
	 * - fixing erroneous messages that do not conform to the ASPN data model, due to e.g. hardware
	 *   malfunction
	 * - preprocessing ASPN measurements of one type into one or more alternative ASPN measurements,
	 *   to make them suitable for processing by currently available measurement processors. For
	 *   example, a raw image might be delivered to pntOS, and then converted into features such
	 *   that a feature-processing measurement processor could utilize it.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_PREPROCESSOR_PLUGIN,
	/**
	 * The last entry in the enum, designed to allow users to know the number of plugin types
	 * defined. This value must not be used by any plugin implementation, other than to find the
	 * length of this enum.
	 */
	PNTOS_NUM_PLUGIN_TYPES
} PntosPluginTypes;

/**
 * An enumeration of the types of fusion that can be performed by pntOS. An implementation of a
 * #PntosFusionPlugin plugin will compare a model from this enum in its
 * PntosFusionPlugin.is_fusion_type_supported function. The return of
 * PntosFusionPlugin.is_fusion_type_supported indicates whether the input type of fusion engine
 * matches the type that will be produced by PntosFusionPlugin.new_fusion_engine.
 *
 * For example, suppose we have a variable `PntosFusionPlugin* plugin`. Then if the return value of
 * `plugin->is_fusion_type_supported(plugin, PNTOS_FUSION_STANDARD_MODEL)` is true, then that means
 * that `plugin->new_fusion_engine(plugin)` will return a `PntosStandardFusionEngine*`.
 */
typedef enum PntosFusionType {
	/**
	 * The standard model of fusion within pntOS. This model assumes that state estimates are
	 * representable in a jointly Gaussian state vector and that updates of the state vector contain
	 * only i.i.d. additive white Gaussian noise. See PntosStandardFusionEngine for more
	 * information.
	 */
	PNTOS_FUSION_STANDARD_MODEL,
	/**
	 * The sampled model of fusion within pntOS. This model assumes that state estimates are
	 * represented by discrete stochastic sample points of a probability density function (i.e.
	 * particles) and that propagate/update functions will receive these samples and be able to
	 * arbitrarily modify each particle's weight, location, and add arbitrary noise to them.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_FUSION_SAMPLED_MODEL,
	/**
	 * The time delayed model of fusion within pntOS. This model assumes that information about a
	 * state is retained across different time epochs and that historical estimate data is available
	 * for processing current time data.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_FUSION_TIME_DELAYED_MODEL,
	/**
	 * The standard model of fusion within pntOS, in compiled format. This model is identical to the
	 * standard model, with the exception that model information is not available in function
	 * pointers on the machine itself but instead binary blobs which have been pre-compiled. This
	 * mode is intended to facilitate usage in environments such as GPGPU filter implementations.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PNTOS_FUSION_STANDARD_COMPILED_MODEL
} PntosFusionType;

/**
 * An enumeration of the types of log outs that are available in pntOS.
 */
typedef enum PntosLoggingLevel {
	/**
	 * This output indicates the program has entered an error state, and likely needs to be
	 * inspected to discover what went wrong.
	 */
	PNTOS_LOG_LEVEL_ERROR,
	/**
	 * This output is designed to warn of a possibly unintended state that may be harmless or be
	 * indicative of a bug.
	 */
	PNTOS_LOG_LEVEL_WARN,
	/**
	 * This output is designed to be informational, and may indicate correct operation.
	 */
	PNTOS_LOG_LEVEL_INFO,
	/**
	 * This output is designed to assist in debugging plugins by providing additional information
	 * about state and behavior which would be otherwise unnecessary.
	 */
	PNTOS_LOG_LEVEL_DEBUG
} PntosLoggingLevel;

/**
 * An enum that specifies the format of data returned/expected in the PntosKeyValueStore.get_raw and
 * PntosKeyValueStore.set_raw methods. This value is otherwise unused when querying a key-value
 * store.
 */
typedef enum PntosKeyValueStoreDataFormat {
	/**
	 * Keys and their corresponding values are returned according to the INI file format
	 * specification.
	 */
	PNTOS_KV_STORE_INI,
	/**
	 * An opaque type that is undefined by the implementer.
	 */
	PNTOS_KV_STORE_UNSPECIFIED
} PntosKeyValueStoreDataFormat;

/**
 * An enum that specifies the supported types in a PntosKeyValueStore. This is
 * useful for keeping track of what type is associated with each value in a
 * PntosKeyValueStore for such functions as ``PntosKeyValueStore::get_type()``.
 * Every value in the PntosKeyValueStore must have an associated
 * PntosKeyValueStoreType - excluding ``PNTOS_KV_STORE_KEY_DNE`` which is
 * intended for error/debug use.
 *
 * Part of the motive for keeping track of the types in the registry is to allow
 * greater compatibility with plugins in other languages (python, for example),
 * as well as greater flexibility in the PntosKeyValueStore implementation. For
 * instance, if all values have an associated type, the PntosKeyValueStore could
 * easily check if a conversion exists between the value in the store and the
 * requested value from a ``get_*()`` call.
 *
 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
 * may change at any time.
 */
typedef enum PntosKeyValueStoreType {
	/*
	 * Indicates a string in the PntosKeyValueStore.
	 * Maps to C type: PntosString
	 */
	PNTOS_KV_STORE_TYPE_STR,
	/*
	 * Indicates an array of strings in the PntosKeyValueStore.
	 * Maps to C type: PntosStringArray
	 */
	PNTOS_KV_STORE_TYPE_STR_ARRAY,
	/*
	 * Indicates an integer in the PntosKeyValueStore.
	 * Maps to C type: int64_t
	 */
	PNTOS_KV_STORE_TYPE_INT,
	/*
	 * Indicates a boolean in the PntosKeyValueStore.
	 * Maps to C type: bool
	 */
	PNTOS_KV_STORE_TYPE_BOOL,
	/*
	 * Indicates a double in the PntosKeyValueStore.
	 * Maps to C type: double
	 */
	PNTOS_KV_STORE_TYPE_DOUBLE,
	/*
	 * Indicates an array of doubles in the PntosKeyValueStore.
	 * Maps to C type: PntosDoubleArray
	 */
	PNTOS_KV_STORE_TYPE_DOUBLE_ARRAY,
	/*
	 * Indicates a message in the PntosKeyValueStore.
	 * Maps to C type: PntosMessage
	 */
	PNTOS_KV_STORE_TYPE_MESSAGE,
	/*
	 * Indicates a raw value in the PntosKeyValueStore.
	 * Maps to C type: PntosByteArray
	 */
	PNTOS_KV_STORE_TYPE_RAW,
	/*
	 * Indicates that the requested key does not exist, and therefore has no
	 * type associated with it.
	 */
	PNTOS_KV_STORE_KEY_DNE
} PntosKeyValueStoreType;

/* TODO: Note error conditions and what happens with type mismatches between set/get. */
/**
 * A key-value store implemented with a string-pair key. Each value can be looked up by an
 * associated key (string). Values can be a variety of different types, depending on which typed
 * function is called. For example, to store a string value "foo" in the key-value store under the
 * key "k1", one would write:
 *
 *     store->set_str(store, "k1", "foo");
 *
 * At this point, the key-value store would have recorded the value into its internal data storage.
 * Later a user could call
 *
 *     store->get_str(store, "k1");
 *
 * To retrieve the value from the store.
 *
 * In general, a PntosKeyValueStore is generated by a PntosRegistry and not directly by other code.
 * The PntosRegistry will return key/value stores on demand, utilizing the data backing store chosen
 * by the plugin to store data (either ephemerally in memory or permanently in persistent storage).
 * In general, it is only valid to call the getters/setters on a PntosKeyValueStore during a batch
 * operation. See PntosRegistry for more information.
 */
typedef struct PntosKeyValueStore {
	PntosManagedMemory* memory;

	/**
	 * Get the array of keys which currently exist in this store. Returns NULL if no keys are
	 * available.
	 */
	PntosStringArray* PNTOS_NULLABLE (*get_key_array)(struct PntosKeyValueStore* self);
	/**
	 * Returns whether or not a given key exists in the store.
	 */
	bool (*has_key)(struct PntosKeyValueStore* self, char* key);
	/**
	 * If the key exists in the store, returns the type corresponding to the
	 * setter which set the value in the store. The type information is returned
	 * as a `PntosKeyValueStoreType` enum. Returns PNTOS_KV_STORE_KEY_DNE if key does
	 * not exist. Some implementations may choose not to implement this function
	 * (leaving the function as a null pointer), so always check if this
	 * function exists before using it.
	 *
	 * **UNSTABLE**: This feature is unstable and is not yet considered part of the stable pntOS
	 * API. Usage of this feature is highly discouraged in non-experimental code, and its definition
	 * may change at any time.
	 */
	PntosKeyValueStoreType (*PNTOS_NULLABLE get_type)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Get the value stored at key as a string. Returns NULL if the key is not available. The return
	 * is guaranteed to not be NULL if called with a valid key, which can be checked with #has_key.
	 */
	PntosString* PNTOS_NULLABLE (*get_str)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Get the value stored at key parsed into an array of strings. Returns NULL if the key is not
	 * available. The return is guaranteed to not be NULL if called with a valid key, which can be
	 * checked with #has_key.
	 */
	PntosStringArray* PNTOS_NULLABLE (*get_str_array)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Get the value stored at \p key as a 64-bit integer. If it is not known whether \p key exists,
	 * #has_key should be used to determine the existence of the key before this function is called
	 * (as this function does not return NULL). This function will attempt to return `INT64_MAX` if
	 * the key does not exist, however users should not rely on this behavior to determine if a key
	 * is available.
	 */
	int64_t (*get_int)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Get the value stored at \p key as a boolean. If it is not known whether \p key exists,
	 * #has_key should be used to determine the existence of the key before this function is called.
	 * This function will attempt to return false if the key does not exist, however users should
	 * not rely on this behavior to determine if a key is available.
	 */
	bool (*get_bool)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Get the value stored at \p key as a double. If the key is not available, returns a `NaN`
	 * value.
	 */
	double (*get_double)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Get the value stored at \p key parsed into an array of doubles. Returns NULL if the key is
	 * not available. The return is guaranteed to not be NULL if called with a valid key, which can
	 * be checked with #has_key.
	 */
	PntosDoubleArray* PNTOS_NULLABLE (*get_double_array)(struct PntosKeyValueStore* self,
	                                                     char* key);
	/**
	 * Get the value stored at \p key as a pointer to a PntosMessage. Returns NULL if the key is not
	 * available or if the value of the stored PntosMessage pointer is NULL.
	 */
	PntosMessage* PNTOS_NULLABLE (*get_message)(struct PntosKeyValueStore* self, char* key);
	/**
	 * Set the given key to the provided value.
	 */
	void (*set_str)(struct PntosKeyValueStore* self, char* key, char* value);
	/**
	 * Set the given key to the provided value. \p num_values must be greater than zero. \p values
	 * is an array of `\0` terminated strings. The length of the \p values array is given by \p
	 * num_values. \p num_values must be greater than zero.
	 */
	void (*set_str_array)(struct PntosKeyValueStore* self,
	                      char* key,
	                      char** values,
	                      size_t num_values);
	/**
	 * Set the given key to the provided value.
	 */
	void (*set_int)(struct PntosKeyValueStore* self, char* key, int64_t value);
	/**
	 * Set the given key to the provided value.
	 */
	void (*set_bool)(struct PntosKeyValueStore* self, char* key, bool value);
	/**
	 * Set the given key to the provided value.
	 */
	void (*set_double)(struct PntosKeyValueStore* self, char* key, double value);
	/**
	 * Set the given key to the provided array of doubles \p values. \p num_values must be greater
	 * than zero. The length of the \p values array is given by \p num_values.
	 */
	void (*set_double_array)(struct PntosKeyValueStore* self,
	                         char* key,
	                         double* values,
	                         size_t num_values);
	/**
	 * Set the given key to the provided value.  A reference to the given \p message will be held
	 * until set_message is called again with the same key or the program ends. \ref set_message can
	 * be called with a NULL \p message to release the reference to the message previously stored in
	 * the given \p key.
	 */
	void (*set_message)(struct PntosKeyValueStore* self,
	                    char* key,
	                    PntosMessage* PNTOS_NULLABLE message);
	/**
	 * Remove the given key from the registry. Returns true if \p key is successfully removed, and
	 * false otherwise. Keys may fail to be removed if the key does not currently exist, or the
	 * backend is unable to remove the key.
	 */
	bool (*remove_key)(struct PntosKeyValueStore* self, char* key);

	/**
	 * The data format that is used by the #set_raw and #get_raw methods.
	 */
	PntosKeyValueStoreDataFormat data_format;
	/**
	 * Set the given key to the provided value.  \p bytes must be formatted to conform to the
	 * definition of a value in #data_format.
	 *
	 * If \p key is NULL, then the contents of \p bytes must include both keys and values and
	 * must be formatted to conform to #data_format.  \p bytes will then be used to set the
	 * corresponding keys and values in the group passed to PntosRegistry.batch_start.
	 *
	 * \p num_bytes is the length in bytes of \p bytes.
	 */
	void (*set_raw)(struct PntosKeyValueStore* self,
	                char* PNTOS_NULLABLE key,
	                unsigned char* bytes,
	                size_t num_bytes);
	/**
	 * Get the value for the given key as an array of bytes.  The format of PntosByteArray.data will
	 * conform to the definition of a value in #data_format. Returns NULL if the given key is not
	 * available. The return is guaranteed to not be NULL if called with a valid key, which can be
	 * checked with #has_key.
	 *
	 * If \p key is NULL, then this function will return all of the keys and values in the group
	 * passed to PntosRegistry.batch_start and will be formatted to conform to keys and values
	 * as defined in #data_format.
	 */
	PntosByteArray* PNTOS_NULLABLE (*get_raw)(struct PntosKeyValueStore* self,
	                                          char* PNTOS_NULLABLE key);
	/**
	 * Ends a batch operation started with a PntosRegistry.batch_start call. After calling this, the
	 * user should not use the PntosKeyValueStore they received from PntosRegistry.batch_start again
	 * without calling \ref batch_restart on the PntosKeyValueStore.
	 *
	 * If keys in the batch were acted upon with \ref set_permanent turned on, and the plugin
	 * supports permanent storage, this call will save changes to permanent storage if \ref
	 * set_permanent is true during the call to \ref batch_end. Enacts equivalent of
	 * `set_permanent(self,false)` before return. If any \ref request_notify observers have been
	 * added, they will be processed prior to this call returning.
	 *
	 * Example: Flushing to permanent storage on #batch_end
	 *
	 *      PntosKeyValueStore* store = registry->batch_start("group");
	 *      ...work...
	 *      store->set_permanent(store,true) ; // if not disabled, flush on batch_end
	 *      ...work...
	 *      store->batch_end(store)          ; // will flush values
	 *
	 * Example: Not flushing to permanent storage on #batch_end
	 *
	 *      PntosKeyValueStore* store = registry->batch_start("group");
	 *      ...work...
	 *      store->set_permanent(store,true)  ; // tag some values
	 *      ...work...
	 *      store->set_permanent(store,false) ; // do not flush on batch_nd
	 *      store->batch_end(store)           ; // will not flush values
	 *
	 * In the second example above, values set with "set" methods after the initial #set_permanent
	 * call are still stored for potential saving to permanent storage.
	 */
	void (*batch_end)(struct PntosKeyValueStore* self);
	/**
	 * Restarts a batch that was previously started with PntosRegistry.batch_start and subsequently
	 * ended with #batch_end. This method is likely much more efficient than
	 * PntosRegistry.batch_start (depending on the registry implementation) as the
	 * PntosRegistry.batch_start method must find the store again given the group name.
	 *
	 * \note While a batch is active, access to the store may be denied to other users. Thus a user
	 * should endeavour to call #batch_end as soon as possible after they are done getting/setting
	 * values in the returned PntosKeyValueStore.
	 */
	void (*batch_restart)(struct PntosKeyValueStore* self);
	/**
	 * Register a callback which gets called each time a key in the store is updated. Allows plugins
	 * to respond asynchronously to parameter updates. Returns true if the notifier was successfully
	 * registered, and false if the store is unable to notify the requester. If key is null, then
	 * the callback will be invoked when any key in the batch's group is modified. Otherwise, the
	 * callback will only be invoked when the given key is modified. The receiver argument, if
	 * non-NULL, will be passed through to the callback's receiver parameter when the callback is
	 * invoked. The receiver argument is designed to allow the caller of #request_notify to pass a
	 * context object through, such that the same context object is available when the callback is
	 * run.
	 *
	 * The \p modified_keys parameter passed to the callback is an array of \p num_modified_keys
	 * `\0` terminated strings. The \p modified_group and \p modified_keys parameters to the
	 * callback are the group and list of keys that were modified respectively. \p
	 * modified_values is a PntosKeyValueStore which can be interrogated for the values of the
	 * modified keys given in \p modified_keys.
	 *
	 * \note The callback must not attempt to set any values inside the PntosKeyValueStore, as the
	 * callback is likely being invoked during the processing of another operation. The callback
	 * should endeavour to store off the updated keys/values as quickly as possible and return,
	 * leaving the processing of the updates to another context or thread when possible. Calling
	 * \ref PntosMediator within the callback may be disallowed by the controller implementation and
	 * lead to undefined behavior.
	 *
	 * \note This method will retain the receiver beyond the lifetime of the function call, as the
	 * purpose of that parameter is to pass it back later in the callback. However, the
	 * PntosKeyValueStore will never dereference the pointer, and thus it is safe to pass in a
	 * receiver that does not survive longer than the lifetime of the function call, as long as the
	 * callback checks for validity of the receiver before using it.
	 */
	bool (*request_notify)(struct PntosKeyValueStore* self,
	                       char* PNTOS_NULLABLE key,
	                       void* PNTOS_NULLABLE receiver,
	                       void (*callback)(void* PNTOS_NULLABLE receiver,
	                                        char* modified_group,
	                                        char** modified_keys,
	                                        size_t num_modified_keys,
	                                        struct PntosKeyValueStore* modified_values));

	/**
	 * Removes a notification as requested by \ref request_notify. The group, receiver, and callback
	 * must match the parameters passed to #request_notify in order to successfully remove a
	 * callback.
	 *
	 * \note This will remove all matching callbacks that have a matching group, receiver, and
	 * callback. If a user registers the same callback twice this will remove both.
	 *
	 * Returns `true` if removal was successful and `false` if it was not. `false` will be returned
	 * if a callback did not exist for the group/receiver combination.
	 */
	bool (*remove_notify)(struct PntosKeyValueStore* self,
	                      char* PNTOS_NULLABLE key,
	                      void* PNTOS_NULLABLE receiver,
	                      void (*callback)(void* PNTOS_NULLABLE receiver,
	                                       char* modified_group,
	                                       char** modified_keys,
	                                       size_t num_modified_keys,
	                                       struct PntosKeyValueStore* modified_values));

	/**
	 * Configure the PntosKeyValueStore to tag values modified with "set" methods as permanently
	 * stored (as opposed to ephemerally stored in memory). Only values acted upon with "set"
	 * methods while #set_permanent is 'true' will be tagged. Values will be flushed according to
	 * registry configuration settings or per \ref batch_end API. Returns the value of the permanent
	 * storage configuration. Callers should check this to verify if the set was successful.
	 *
	 * Example: Tagging specific keys to be permanently stored
	 *
	 *      PntosKeyValueStore* store = registry->batch_start("group");
	 *      store->set_double("key1",1234.56);  // does not tag this value as permanently stored
	 *      store->set_permanent(store,true) ;  // start tagging set* calls as permanently stored
	 *      store->set_double("key1",987.65) ;
	 *      store->set_int("key2",123)       ;  // both key1 and key2 values tagged
	 *      store->set_permanent(store,false);  // disable permanent storage
	 *      store->set_double("key1",456.78) ;  // key1 = 456.78 is value of key1 in store
	 *                                          // key1 = 987.65 tagged to be permanently stored
	 *                                          // key2 = 123    tagged to be permanently stored
	 **/
	bool (*set_permanent)(struct PntosKeyValueStore* self, bool permanent);

} PntosKeyValueStore;

/**
 * A registry of key/value data which is organized by (string) groups. In order to get/set a key in
 * the registry, one must call PntosRegistry.batch_start with the group the key is stored under and
 * then use the resulting PntosKeyValueStore to get/set the key/value pair. When one is done
 * accessing keys in the PntosKeyValueStore, they must call PntosKeyValueStore.batch_end. It is not
 * permitted to access any member inside the PntosKeyValueStore after a batch has ended. If a
 * user has ended a batch and then desires to access the PntosKeyValueStore again, they may use the
 * PntosKeyValueStore.batch_restart method.
 */
typedef struct PntosRegistry {
	PntosManagedMemory* memory;

	/**
	 * Begin a batch get/set operation wherein the user may make any number of modifications to the
	 * keys/values in the \p group. The registry implementation may wait to batch these requests
	 * until PntosKeyValueStore.batch_end is called for better performance. For example, a lock may
	 * be obtained at the beginning of a #batch_start and not released until a
	 * PntosKeyValueStore.batch_end call is encountered. Thus, a plugin that calls #batch_start
	 * should endeavour to make its calls to the "set_", "get_", and "register" methods as quickly
	 * as possible and call PntosKeyValueStore.batch_end immediately, as doing otherwise may be
	 * locking other plugins out of access to the registry (depending on the registry plugin
	 * implementation). If a plugin supports PntosKeyValueStore.request_notify, then notifications
	 * of updates may be suspended until the batch ends. After a batch is ended, the returned
	 * PntosKeyValueStore can still be used to access the store via
	 * PntosKeyValueStore.batch_restart.
	 *
	 * \note While a batch is active, access to the store may be denied to other users. Thus a user
	 * should endeavour to call PntosKeyValueStore.batch_end as soon as possible after they are done
	 * getting/setting values in the returned PntosKeyValueStore.
	 */
	PntosKeyValueStore* (*batch_start)(struct PntosRegistry* self, char* group);
	/**
	 * Get the array of groups which currently exist. Returns NULL if no groups exist.
	 */
	PntosStringArray* PNTOS_NULLABLE (*get_group_array)(struct PntosRegistry* self);

	/**
	 * Returns whether or not a given group has had any values added to it (for any key).
	 */
	bool (*has_group)(struct PntosRegistry* self, char* group);

	/**
	 * Register a callback which gets called each time a new group is made in the registry. Returns
	 * true if the notifier was successfully registered, and false if the registry is unable to
	 * notify the requester. The callback will receive the same receiver as was passed into this
	 * method, which may be used as a context object. The \p new_group parameter passed into the
	 * callback is the name of the new group that was created.
	 *
	 * \note This method will retain the receiver beyond the lifetime of the function call, as the
	 * purpose of that parameter is to pass it back later in the callback. However, the method will
	 * never dereference the receiver pointer, and thus it is safe to pass in a receiver that does
	 * not survive longer than the lifetime of the function call, as long as the callback checks for
	 * validity of the receiver before using it.
	 */
	bool (*request_notify_new_group)(struct PntosRegistry* self,
	                                 void* receiver,
	                                 void (*callback)(void* receiver, char* new_group));

} PntosRegistry;

/**
 * A set of callbacks which are handed to a pntOS plugin upon initialization.
 *
 * When a plugin is first initialized into pntOS, it is guaranteed that the plugin will be passed an
 * instance of this struct via an invocation of PntosCommonPlugin.init_plugin (see PntosCommonPlugin
 * for more information). The plugin may then use the set of function calls in this struct to make
 * requests of the controller.
 *
 * All of the functions on this struct (and any returned values from those functions) are guaranteed
 * to be thread-safe for use by all plugins. Thus, after a pntOS plugin has received a copy of a
 * PntosMediator it can freely call the functions contained therein without doing any explicit
 * locking. This thread safety is implemented by the controller when it creates the mediator before
 * passing them to other plugins.

 * Callers must still take care to only call functions in \ref PntosMediator which they are not
 * themselves responsible for implementing. The details of which plugins are used in the
 * implementation of any particular function on this struct is decided by the \ref
 * PntosControllerPlugin, and thus is implementation specific to the controller used.
 */
typedef struct PntosMediator {
	PntosManagedMemory* memory;

	/**
	 * Request a list of strings describing the solutions available. One of these description
	 * strings may be used when calling #request_solutions. For consistency, these strings should
	 * adhere to the following conventions:
	 *
	 * - Strings should be upper case and have words and acronyms separated by underscores
	 *   (`UPPER_SNAKE_CASE`).
	 * - Strings should contain the substring `BEST` when they represent the primary solution.
	 * - Strings should contain the substring DEAD_RECKONING when they represent a solution suitable
	 *   for estimating relative motion or rotation over a period of time. This solution may drift
	 *   more than BEST solutions, as the goal is to allow a user to get an estimate of the relative
	 *   motion between different times. In the calculation of this solution, some sensor
	 *   measurement might be excluded. For example, a system with an IMU might provide a
	 *   DEAD_RECKONING solution which is the solution from its free-running inertial mechanization,
	 *   with resets disabled during the time intervals between solution_times (but resets applied
	 *   before all of the solution_times).
	 * - Strings should include a substring indicating the type of solution returned. This substring
	 *   should contain the string-equivalent to the AspnMessageType enum value, followed by the
	 *   string `_ESTIMATE`. This allows the user to perform substring matching without a risk of
	 *   getting a false positive match from a type whose string would be a subset of another type.
	 *
	 * For example, if the primary solution is an ASPN PVA then the string
	 * `MY_BEST_ASPN_MEASUREMENT_POSITION_VELOCITY_ATTITUDE_ESTIMATE` would fulfill the convention.
	 *
	 * These conventions allow the user to identify their desired type of solution using substring
	 * matching.
	 */
	PntosStringArray* (*get_filter_description_list)(struct PntosMediator* self);

	/**
	 * Request filtering solutions at the times specified in the array \p solution_times. The number
	 * of time entries in \p solution_times is specified by \p num_solution_times.
	 *
	 * To select which filter(s) to request solutions from, enter a valid filter description string
	 * in \p filter_description. Valid filter description strings can be obtained by calling
	 * get_filter_description_list(). Passing in NULL will provide a result specific to a particular
	 * implementation. When \p filter_description is NULL, the implementation should endeavor to
	 * return its best solution.
	 *
	 * Returned will be an array of messages containing the filter solutions for the requested \p
	 * solution_times. The number of solutions should equal \p num_solution_times, although some
	 * entries may be NULL if they are unavailable at the corresponding time in \p solution_times.
	 * The returned PntosMessageArray may be NULL if \p filter_description is invalid.
	 */
	PntosMessageArray* PNTOS_NULLABLE (*request_solutions)(struct PntosMediator* self,
	                                                       AspnTypeTimestamp* solution_times,
	                                                       size_t num_solution_times,
	                                                       char* PNTOS_NULLABLE filter_description);

	/**
	 * Send a new message to the system for arbitrary processing. For example, this function is
	 * useful for plugins who have just received new sensor data that they wish to relay to the
	 * system to be used in a sensor fusion solution.
	 */
	void (*process_pntos_message)(struct PntosMediator* self, PntosMessage* message);
	/**
	 * Request that pntOS broadcast the provided message out to the network. The \p
	 * destination_identifier parameter is a transport-specific identifier that allows transports to
	 * determine how to route the message. If the destination transport has the concept of a channel
	 * or topic, \p destination_identifier should be populated by the channel or topic. Otherwise,
	 * the identifier is populated in a plugin-specific manner defined by the destination transport.
	 * If \p destination_identifier is NULL, then the transport should output the message in the
	 * "default" output channel/topic and route being used by pntOS.
	 *
	 * The transport parameter is the C string identifier of a transport plugin that the message
	 * should be routed to. The transport parameter should match the PntosCommonPlugin.identifier
	 * string of a PntosTransportPlugin active in the system. If the transport parameter is NULL,
	 * this indicates that the message should be broadcast to all available transports.
	 */
	void (*broadcast_aspn_message)(struct PntosMediator* self,
	                               PntosMessage* message,
	                               char* PNTOS_NULLABLE transport,
	                               char* PNTOS_NULLABLE destination_identifier);
	/**
	 * Send a loggable message to the system, to be logged through the current logging
	 * infrastructure enabled (e.g. the console, a logfile, etc.).
	 */
	void (*log_message)(struct PntosMediator* self, PntosLoggingLevel level, char* message);

	/**
	 * Send a loggable message to the system, to be logged through the current logging
	 * infrastructure enabled (e.g. the console, logfile, etc.). The \p fmt parameter and message
	 * parameters are interpreted as `printf(fmt, message)` arguments.
	 */
	void (*log_message_fmt)(struct PntosMediator* self, PntosLoggingLevel level, char* fmt, ...);

	/**
	 * A pointer to a PntosRegistry object that can be used to update keys/values in the pntOS
	 * global registry.
	 */
	PntosRegistry* registry;
} PntosMediator;

/**
 * Common definitions that all plugins must provide. This structure should not be used directly
 * (except in the case of a utility plugin), but instead is composed as the first field on all of
 * the concrete pntOS plugin structures. For example, the transport plugin is specified as:
 *
 *     struct PntosTransportPlugin {
 *         PntosCommonPlugin common;
 *         ...
 *     } PntosTransportPlugin;
 *
 * Thus this structure defines a set of fields that all plugins have. The
 * PntosCommonPlugin.init_plugin function is guaranteed to be called by pntOS when the plugin is
 * first loaded into memory by the system.
 *
 * When defining a new e.g. transport plugin, the plugin writer is responsible for implementing all
 * fields on the `PntosTransportPlugin` structure. Thus, the fields of the `PntosCommonPlugin`
 * nested on the `PntosTransportPlugin` are implemented by the plugin writer.
 */
typedef struct PntosCommonPlugin {
	PntosManagedMemory* memory;
	/**
	 * A function that will be called by pntOS once and only once when it first initializes the
	 * plugin before any other functions on the plugin are called. Here the plugin may do dynamic
	 * runtime initialization of its members, and is given the full path to the location of a data
	 * folder specific to the plugin, in case it needs to acquire additional files. A pointer to an
	 * instance of #PntosMediator will be passed which the plugin should save off for later use.
	 * Whenever the plugin needs to make a request of pntOS, it should use one of the fields in the
	 * `PntosMediator` instance received by the plugin in this function call.
	 *
	 * Implementation note:
	 *
	 * This inversion of control allows the controller to implement the `PntosMediator` struct, and
	 * abstracts away the return communication channel from the plugin to the rest of the system.
	 * Thus, the plugin need only implement `PntosMediator` by simply saving a copy of the functions
	 * that the controller passes into it. Then, when the plugin later needs to make requests of the
	 * system, it may call a function in its copy of `PntosMediator`, without needing any knowledge
	 * of how the controller implemented `PntosMediator`. This allows controllers to implement
	 * arbitrary concurrency models, including single-threaded, multi-threaded, multi-process, and
	 * distributed computing.
	 *
	 * \p plugin_resources_location specifies the location of the plugin's resources.  The location
	 * is determined by the controller plugin, and therefore is controller implementation specific.
	 * Plugin implementers wishing to provide a resource to their plugin should consult the
	 * documentation of the controller to determine which location scheme will be passed into this
	 * function.
	 *
	 * \p mediator is NULL-able if the plugin type being initialized is a PntosControllerPlugin.
	 * Non-controller plugins may assume that the mediator parameter is not NULL.
	 */
	void (*init_plugin)(struct PntosCommonPlugin* self,
	                    char* PNTOS_NULLABLE plugin_resources_location,
	                    PntosMediator* PNTOS_NULLABLE mediator);

	/**
	 * A function that will be called by pntOS when it is done using the plugin. Here the plugin
	 * should release any resources it has acquired (including the `PntosMediator` if it kept a
	 * reference to that when #init_plugin was called). When this function call returns pntOS may
	 * only call the destructor function (it will not call any other functions of this plugin). The
	 * plugin may not call any function on any other plugin, mediator, or use any resource that was
	 * given to it by pntOS after it returns from this function.
	 */
	void (*shutdown_plugin)(struct PntosCommonPlugin* self);

	/**
	 * The type of this plugin. This field is used to determine what types a parent struct is
	 * castable to, when `PntosCommonPlugin` is the first field in the parent struct. For example,
	 * suppose we defined a new pntOS plugin called `FooPlugin`, and composed a `PntosCommonPlugin`
	 * inside of it:
	 *
	 *     typedef struct PntosFooPlugin {
	 *         PntosCommonPlugin common;
	 *         .. other fields
	 *     } PntosFooPlugin;
	 *
	 * Then we had a function defined that takes in arbitrary plugin types:
	 *
	 *     void some_function(PntosCommonPlugin * my_plugin)
	 *
	 * We could use the `plugin_type` field to identify what type of plugin we really received and
	 * cast it back in the implementation of `some_function`:
	 *
	 *     void some_function(PntosCommonPlugin * my_plugin) {
	 *         if (my_plugin->plugin_type == FOO_PLUGIN_TYPE) {
	 *             PntosFooPlugin * foo_plugin = (PntosFooPlugin *) my_plugin;
	 *             .. use FooPlugin functionality ..
	 *         }
	 *     }
	 */
	PntosPluginTypes plugin_type;
	/**
	 * A string identifier uniquely identifying this plugin. This string will be used to determine
	 * the unique space this plugin receives in the system config.
	 */
	char* identifier;
} PntosCommonPlugin;

/**
 * A container that stores an array of pntOS plugins of arbitrary type.
 */
typedef struct PntosPluginArray {
	PntosManagedMemory* memory;
	PntosCommonPlugin** plugins;
	size_t num_plugins;

} PntosPluginArray;


PNTOS_ASSUME_NONNULL_END

#ifdef __cplusplus
}
#endif
